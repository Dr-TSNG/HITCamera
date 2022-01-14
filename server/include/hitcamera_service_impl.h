#pragma once

#include <iremote_stub.h>
#include <system_ability.h>
#include "hitcamera_service_stub.h"

namespace OHOS {

    class HITCameraService : public SystemAbility, public HITCameraServiceStub {
        DECLARE_SYSTEM_ABILITY(HITCameraService);

    public:
        DISALLOW_COPY_AND_MOVE(HITCameraService);

        explicit HITCameraService(int32_t systemAbilityId, bool runOnCreate = true);

        int32_t Capture(PictureHandle& handle) override;
    };
}
