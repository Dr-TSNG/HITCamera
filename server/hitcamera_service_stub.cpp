#include "hitcamera_service_stub.h"

namespace OHOS::HITCamera {

    int HITCameraServiceStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data, OHOS::MessageParcel& reply,
                                                    OHOS::MessageOption& option) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
