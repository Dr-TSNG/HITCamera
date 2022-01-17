#include "hitcamera_service_stub.h"

namespace OHOS::HITCamera {

    int HITCameraServiceStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data, OHOS::MessageParcel& reply,
                                                    OHOS::MessageOption& option) {
        int errCode = ERR_NONE;
        switch (code) {
            case ACTION_CAPTURE:
                errCode = HITCameraServiceStub::HandleCapture(reply);
                break;
            default:
                LOGE("HITCameraServiceStub request code %d not handled", code);
                break;
        }
        return errCode;
    }

    int HITCameraServiceStub::HandleCapture(MessageParcel& data, MessageParcel& reply) {
        sptr<Ashmem> ashmem;
        uint32_t width = data.ReadUint32();
        uint32_t height = data.ReadUint32();
        int error = Capture(ashmem, width, height);
        if (error != ERR_NONE) {
            if (ashmem != nullptr) {
                ashmem->UnmapAshmem();
                ashmem->CloseAshmem();
            }
            return error;
        }

        if (!reply.WriteAshmem(ashmem)) {
            LOGE("%s", "HITCameraServiceStub HandleCapture failed to write ashmem");
            return IPC_STUB_WRITE_PARCEL_ERR;
        }

        return ERR_NONE;
    }
}
