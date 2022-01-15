#include "hitcamera_service_proxy.h"

namespace OHOS::HITCamera {

    HITCameraServiceProxy::HITCameraServiceProxy(const sptr<IRemoteObject>& impl)
            : IRemoteProxy<IHITCameraService>(impl) {}

    sptr<PictureHandle> HITCameraServiceProxy::Capture() {
        // TODO
        return nullptr;
    }
}
