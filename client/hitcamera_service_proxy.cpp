#include "hitcamera_service_proxy.h"

namespace OHOS {

    HITCameraServiceProxy::HITCameraServiceProxy(const sptr<IRemoteObject>& impl)
            : IRemoteProxy<IHITCameraService>(impl) {}

    int32_t HITCameraServiceProxy::Capture(PictureHandle& handle) {
        // TODO
        return 0;
    }
}
