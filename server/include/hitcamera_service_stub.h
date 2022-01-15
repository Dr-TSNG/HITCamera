#pragma once

#include <iremote_stub.h>
#include "ihitcamera_service.h"

namespace OHOS::HITCamera {

    class HITCameraServiceStub : public IRemoteStub<IHITCameraService> {
    public:
        int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    };
}
