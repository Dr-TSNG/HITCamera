#pragma once

#include <refbase.h>
#include <iremote_broker.h>

namespace OHOS {

    struct PictureHandle {
        size_t size;
        sptr<uint8_t[]> content;
    };

    class IHITCameraService : public IRemoteBroker {
    public:
        virtual int32_t Capture(PictureHandle& handle) = 0;
    };
}
