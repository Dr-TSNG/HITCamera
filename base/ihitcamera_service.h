#pragma once

#include <refbase.h>
#include <iremote_broker.h>
#include "hitbase.h"

namespace OHOS::HITCamera {

    enum Codes {
        CAPTURE
    };

    class IHITCameraService : public IRemoteBroker {
    public:
        virtual int Capture(sptr<PictureHandle>& handle) = 0;
    };
}
