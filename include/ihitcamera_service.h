#pragma once

#include <refbase.h>
#include <iremote_broker.h>
#include "hitbase.h"

namespace OHOS::HITCamera {

    class IHITCameraService : public IRemoteBroker {
    public:
        virtual int Capture(sptr<Ashmem>& ashmem, uint32_t width, uint32_t height) = 0;

        DECLARE_INTERFACE_DESCRIPTOR(u"IHITCameraService");
    };
}
