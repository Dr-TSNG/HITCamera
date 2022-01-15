#include "hitcamera_service_stub.h"

namespace OHOS::HITCamera {

    int HITCameraServiceStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data, OHOS::MessageParcel& reply,
                                                    OHOS::MessageOption& option) {
        int errCode = ERR_NONE;
        switch (code) {
            case CAPTURE:
                errCode = HITCameraServiceStub::HandleCapture(reply);
                break;
            default:
                LOGE("HITCameraServiceStub request code %d not handled", code);
                break;
        }
        return errCode;
    }

    int HITCameraServiceStub::HandleCapture(MessageParcel& reply) {
        sptr<PictureHandle> handle;
        int errCode = HITCameraServiceStub::Capture(handle);
        if (errCode != ERR_NONE) return errCode;

        if (!reply.WriteUint64(handle->size)) {
            LOGE("%s", "HITCameraServiceStub HandleCapture failed to write size");
            return IPC_STUB_WRITE_PARCEL_ERR;
        }

        if (!reply.WriteBuffer(handle->content, handle->size)) {
            LOGE("%s", "HITCameraServiceStub HandleCapture failed to write content");
            return IPC_STUB_WRITE_PARCEL_ERR;
        }

        return ERR_NONE;
    }
}
