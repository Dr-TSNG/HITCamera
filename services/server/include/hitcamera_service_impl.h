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

        int Capture(sptr<Ashmem>& ashmem, uint32_t width, uint32_t height) override;

        void OnStart() override;

        void OnDump() override;

        void OnStop() override;
    };
}
