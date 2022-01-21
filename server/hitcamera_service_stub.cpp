#include "hitcamera_service_stub.h"

namespace OHOS::HITCamera {

    int HITCameraServiceStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel& data, OHOS::MessageParcel& reply,
                                                    OHOS::MessageOption& option) {
        int error = ERR_NONE;
        switch (code) {
            case ACTION_CAPTURE:
                error = HITCameraServiceStub::HandleCapture(data, reply);
                break;
            default:
                LOGE("HITCameraServiceStub request code %d not handled", code);
                break;
        }
        return error;
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
            LOGE("HITCameraServiceStub HandleCapture failed to write ashmem");
            ashmem->UnmapAshmem();
            ashmem->CloseAshmem();
            return IPC_STUB_WRITE_PARCEL_ERR;
        }

        ashmem->UnmapAshmem();
        ashmem->CloseAshmem();
        return ERR_NONE;
    }
}
