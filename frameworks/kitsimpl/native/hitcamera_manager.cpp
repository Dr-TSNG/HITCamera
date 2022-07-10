#include <cstring>
#include <iservice_registry.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "hitcamera_manager.h"
#include "udp_piece.h"

namespace OHOS::HITCamera {

    CameraManager* CameraManager::getInstance() {
        static CameraManager* mManager = nullptr;
        if (mManager == nullptr) {
            auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            sptr<IRemoteObject> object = samgr->GetSystemAbility(9902);
            if (object == nullptr) {
                LOGE("Get SystemAbility failed");
                return nullptr;
            }
            auto proxy = iface_cast<IHITCameraService>(object);
            if (proxy == nullptr) {
                LOGE("Cast proxy failed");
                return nullptr;
            }
            mManager = new CameraManager(std::move(proxy));
        }
        return mManager;
    }

    std::variant<PictureHandle, int> CameraManager::Capture(uint32_t width, uint32_t height) {
        sptr<Ashmem> ashmem;
        int error = mServiceProxy->Capture(ashmem, width, height);
        if (error != NO_ERROR) {
            LOGE("Failed to capture picture: %{public}d", error);
            return error;
        }
        int fd = ashmem->GetAshmemFd();
        int32_t size = ashmem->GetAshmemSize();
        ashmem->MapReadOnlyAshmem();
        const void* buf = ashmem->ReadFromAshmem(size, 0);
        mShmMap[fd] = ashmem;
        LOGI("Capture a new picture: %{public}d", fd);
        return PictureHandle{fd, size, reinterpret_cast<int64_t>(buf)};
    }

    int CameraManager::BindRemote(int port) {
        mRemoteSock = socket(PF_INET, SOCK_DGRAM, 0);
        if (mRemoteSock < 0) {
            LOGE("Failed to create socket: %{public}d", errno);
            return Codes::FAILED_CREATE_SOCKET;
        }
        struct sockaddr_in servaddr{
                .sin_family = AF_INET,
                .sin_port = htons(port),
                .sin_addr = {.s_addr = htonl(INADDR_ANY)}
        };
        if (-1 == bind(mRemoteSock, (struct sockaddr*) &servaddr, sizeof(servaddr))) {
            LOGE("Failed to bind socket: %{public}d", errno);
            return Codes::FAILED_BIND_SOCKET;
        }
        LOGI("Listen on port: %{public}d", port);
        mUdpPiece = std::make_unique<UdpPiece>(1024 * 1024 * 3);
        return ERR_OK;
    }

    std::variant<PictureHandle, int> CameraManager::GetRemote() const {
        uint8_t recvbuf[1024];
        struct sockaddr_in peeraddr{};
        socklen_t peerlen;
        mUdpPiece->Reset();
        while (true) {
            peerlen = sizeof(peeraddr);
            memset(recvbuf, 0, sizeof(recvbuf));
            ssize_t n = recvfrom(mRemoteSock, recvbuf, sizeof(recvbuf), 0,
                                 (struct sockaddr*) &peeraddr, &peerlen);
            if (n <= 0) {
                if (errno == EINTR) continue;
                return Codes::FAILED_RECV_SOCKET;
            }
            int ret = mUdpPiece->Merge(recvbuf, n);
            if (ret == 1) {
                auto* pic = new uint8_t[mUdpPiece->GetRecvLen()];
                memcpy(pic, mUdpPiece->GetRecvBuf(), mUdpPiece->GetRecvLen());
                return PictureHandle{-1, mUdpPiece->GetRecvLen(), int64_t(pic)};
            } else if (ret == -1) {
                return Codes::FAILED_MERGE_PACKAGE;
            }
        }
    }

    void CameraManager::UnbindRemote() {
        if (mRemoteSock > 0) {
            close(mRemoteSock);
            mRemoteSock = 0;
        }
        mUdpPiece.reset();
    }

    void CameraManager::Release(PictureHandle handle) {
        if (handle.id == -1) {
            delete[] (uint8_t*) handle.buffer;
        } else {
            sptr<Ashmem> ashmem = mShmMap[handle.id];
            ashmem->UnmapAshmem();
            ashmem->CloseAshmem();
            mShmMap.erase(handle.id);
            LOGI("Release picture handle %{public}d", handle.id);
        }
    }
}
