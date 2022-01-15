#pragma once

#include <refbase.h>
#include <iremote_broker.h>
#include "hitbase.h"

namespace OHOS::HITCamera {

    class IHITCameraService : public IRemoteBroker {
    public:
        virtual sptr<PictureHandle> Capture() = 0;
    };
}
