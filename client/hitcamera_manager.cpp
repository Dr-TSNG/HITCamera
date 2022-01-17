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

    PictureHandle CameraManager::Capture(uint32_t width, uint32_t height) {
        sptr<Ashmem> ashmem;
        int error = mServiceProxy->Capture(ashmem, width, height);
        if (error != NO_ERROR) return PictureHandle{-1, 0, nullptr};
        int fd = ashmem->GetAshmemFd();
        int32_t size = ashmem->GetAshmemSize();
        mShmMap[fd] = ashmem;
        return PictureHandle{fd, size, ashmem->ReadFromAshmem(size, 0)};
    }

    void CameraManager::Release(int fd) {
        sptr<Ashmem> ashmem = mShmMap[fd];
        ashmem->UnmapAshmem();
        ashmem->CloseAshmem();
        mShmMap.erase(fd);
    }
}
