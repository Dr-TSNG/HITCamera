#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    HITCameraServiceProxy::HITCameraServiceProxy(const sptr<IRemoteObject>& impl)
            : IRemoteProxy<IHITCameraService>(impl) {}

    int HITCameraServiceProxy::Capture(sptr<Ashmem>& ashmem, uint32_t width, uint32_t height) {
        MessageParcel data;
        MessageParcel reply;
        MessageOption option;

        data.WriteUint32(width);
        data.WriteUint32(height);
        int error = Remote()->SendRequest(ACTION_CAPTURE, data, reply, option);
        if (error != ERR_NONE) {
            LOGE("HITCameraServiceProxy Capture failed, error: %{public}d", error);
            ashmem = nullptr;
            return error;
        }

        ashmem = reply.ReadAshmem();
        if (ashmem == nullptr) {
            LOGE("HITCameraServiceProxy Capture failed, error: %{public}d", IPC_PROXY_ERR);
            return IPC_PROXY_ERR;
        }

        return NO_ERROR;
    }
}
