#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    HITCameraServiceProxy::HITCameraServiceProxy(const sptr<IRemoteObject>& impl)
            : IRemoteProxy<IHITCameraService>(impl) {}

    int HITCameraServiceProxy::Capture(sptr<PictureHandle>& handle) {
        // TODO
        return NO_ERROR;
    }
}
