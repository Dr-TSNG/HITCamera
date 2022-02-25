#pragma once

#include <unordered_map>
#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    struct PictureHandle {
        int32_t id;
        int32_t size;
        int64_t buffer;
    };

    class CameraManager {
        DISALLOW_COPY_AND_MOVE(CameraManager);

    public:
        static CameraManager& Instance();

        PictureHandle Capture(uint32_t width, uint32_t height);

        void Release(PictureHandle handle);

    private:
        sptr<IHITCameraService> mServiceProxy = nullptr;

        std::unordered_map<int, sptr<Ashmem>> mShmMap;

        CameraManager();
    };
}