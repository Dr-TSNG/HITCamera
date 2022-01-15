#pragma once

#include <iremote_stub.h>
#include <system_ability.h>
#include "hitcamera_service_stub.h"

namespace OHOS::HITCamera {

    class HITCameraService : public SystemAbility, public HITCameraServiceStub {
        DECLARE_SYSTEM_ABILITY(HITCameraService);

    public:
        DISALLOW_COPY_AND_MOVE(HITCameraService);

        explicit HITCameraService(int32_t systemAbilityId, bool runOnCreate = true);

        sptr<PictureHandle> Capture() override;
    };
}
