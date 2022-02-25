#pragma once

#include <iremote_proxy.h>
#include "ihitcamera_service.h"

namespace OHOS::HITCamera {

    class HITCameraServiceProxy : public IRemoteProxy<IHITCameraService> {
    public:
        explicit HITCameraServiceProxy(const sptr<IRemoteObject>& impl);

        int Capture(sptr<Ashmem>& ashmem, uint width, uint height) override;

    private:
        static inline BrokerDelegator<HITCameraServiceProxy> delegator_;
    };
}
