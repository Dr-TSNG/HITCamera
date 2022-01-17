#include <climits>
#include <iservice_registry.h>
#include "hitcamera_manager.h"

namespace OHOS::HITCamera {

    CameraManager::CameraManager() {
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> object = samgr->GetSystemAbility(9902);
        if (object == nullptr) {
            fprintf(stderr, "Get HITCameraService failed\n");
            return;
        }
        mServiceProxy = iface_cast<IHITCameraService>(object);
        if (mServiceProxy == nullptr) {
            fprintf(stderr, "Cast proxy failed\n");
            return;
        }
    }

    CameraManager& CameraManager::Instance() {
        static CameraManager mManager = CameraManager();
        return mManager;
    }

    const char* CameraManager::Capture(uint32_t width, uint32_t height) {
        sptr<Ashmem> ashmem;
        int error = mServiceProxy->Capture(ashmem, width, height);
        if (error != NO_ERROR) return nullptr;
        int fd = ashmem->GetAshmemFd();
        char* path = new char[PATH_MAX];
        sprintf(path, "/proc/fd/%d", fd);

        mShmMap[fd] = ashmem;
        mPathMap[fd] = path;
        return path;
    }

    void CameraManager::Release(const char* path) {
        int fd;
        sscanf(path, "/proc/fd/%d", &fd);
        sptr<Ashmem> ashmem = mShmMap[fd];
        delete[] mPathMap[fd];
        ashmem->UnmapAshmem();
        ashmem->CloseAshmem();

        mShmMap.erase(fd);
        mPathMap.erase(fd);
    }
}
