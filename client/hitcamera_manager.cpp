#include <iservice_registry.h>
#include "hitcamera_manager.h"

namespace OHOS::HITCamera {

    CameraManager::CameraManager() {
        auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        sptr<IRemoteObject> object = samgr->GetSystemAbility(9902);
        if (object == nullptr) {
            LOGE("Get HITCameraService failed");
            return;
        }
        mServiceProxy = iface_cast<IHITCameraService>(object);
        if (mServiceProxy == nullptr) {
            LOGE("Cast proxy failed");
            return;
        }
    }

    CameraManager& CameraManager::Instance() {
        static CameraManager mManager = CameraManager();
        return mManager;
    }

    PictureHandle CameraManager::Capture(uint32_t width, uint32_t height) {
        sptr<Ashmem> ashmem;
        int error = mServiceProxy->Capture(ashmem, width, height);
        if (error != NO_ERROR) return PictureHandle{-1, 0, 0};
        int fd = ashmem->GetAshmemFd();
        int32_t size = ashmem->GetAshmemSize();
        ashmem->MapReadOnlyAshmem();
        const void* buf = ashmem->ReadFromAshmem(size, 0);
        mShmMap[fd] = ashmem;
        LOGI("Capture a new picture: %d", fd);
        return PictureHandle{fd, size, reinterpret_cast<int64_t>(buf)};
    }

    void CameraManager::Release(PictureHandle handle) {
        sptr<Ashmem> ashmem = mShmMap[handle.id];
        ashmem->UnmapAshmem();
        ashmem->CloseAshmem();
        mShmMap.erase(handle.id);
        LOGI("Release picture handle %d", handle.id);
    }
}
