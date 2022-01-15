#pragma once

#include <iremote_proxy.h>
#include "ihitcamera_service.h"

namespace OHOS::HITCamera {

    class HITCameraServiceProxy : public IRemoteProxy<IHITCameraService> {
    public:
        explicit HITCameraServiceProxy(const sptr<IRemoteObject> &impl);

        sptr<PictureHandle> Capture() override;
    };
}
