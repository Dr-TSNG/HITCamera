#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    HITCameraServiceProxy::HITCameraServiceProxy(const sptr<IRemoteObject>& impl)
            : IRemoteProxy<IHITCameraService>(impl) {}

    int HITCameraServiceProxy::Capture(sptr<PictureHandle>& handle) {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        int error = Remote()->SendRequest(CAPTURE, data, reply, option);
        if (error != ERR_NONE) {
            LOGE("HITCameraServiceProxy Capture failed, error: %d", error);
            handle = nullptr;
            return error;
        }

        handle = new PictureHandle;
        handle->size = reply.ReadUint64();
        handle->content = reply.ReadBuffer(handle->size);

        return NO_ERROR;
    }
}
