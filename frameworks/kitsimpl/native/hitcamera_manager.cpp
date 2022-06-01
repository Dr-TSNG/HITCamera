#include <iservice_registry.h>
#include "hitcamera_manager.h"

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

    void CameraManager::Release(PictureHandle handle) {
        sptr<Ashmem> ashmem = mShmMap[handle.id];
        ashmem->UnmapAshmem();
        ashmem->CloseAshmem();
        mShmMap.erase(handle.id);
        LOGI("Release picture handle %{public}d", handle.id);
    }
}
