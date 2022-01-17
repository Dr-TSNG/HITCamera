#pragma once

#include <unordered_map>
#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    class CameraManager {
        DISALLOW_COPY_AND_MOVE(CameraManager);

    public:
        static CameraManager& Instance();

        const char* Capture(uint32_t width, uint32_t height);

        void Release(const char* path);

    private:
        sptr<IHITCameraService> mServiceProxy = nullptr;

        std::unordered_map<int, sptr<Ashmem>> mShmMap;
        std::unordered_map<int, char*> mPathMap;

        CameraManager();
    };
}
