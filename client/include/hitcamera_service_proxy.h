#pragma once

#include <iremote_proxy.h>
#include "ihitcamera_service.h"

namespace OHOS {

    class HITCameraServiceProxy : public IRemoteProxy<IHITCameraService> {
    public:
        explicit HITCameraServiceProxy(const sptr<IRemoteObject> &impl);

        int32_t Capture(PictureHandle& handle) override;
    };
}
