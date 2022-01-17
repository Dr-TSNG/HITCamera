#include "take_picture.h"
#include "hitcamera_service_impl.h"
#include "system_ability.h"

namespace OHOS::HITCamera {

    REGISTER_SYSTEM_ABILITY_BY_ID(HITCameraService, 9902, true);

    HITCameraService::HITCameraService(int32_t systemAbilityId, bool runOnCreate)
            : SystemAbility(systemAbilityId, runOnCreate) {
        LOGD("%s", "HITCameraService Instantiate");
    }

    void HITCameraService::OnStart() {
        LOGD("%s", "HITCameraService::OnStart()");
        Publish(this);
    }

    void HITCameraService::OnDump() {
        LOGD("%s", "HITCameraService::OnDump()");
    }

    void HITCameraService::OnStop() {
        LOGD("%s", "HITCameraService::OnStop()");
    }

    int HITCameraService::Capture(sptr<Ashmem>& ashmem, uint32_t width, uint32_t height) {
        auto res = TakeOnePicture(width, height);
        if (auto shm = std::get_if<sptr<Ashmem>>(&res)) {
            ashmem = *shm;
            return NO_ERROR;
        } else if (auto err = std::get_if<int>(&res)) {
            ashmem = nullptr;
            return *err;
        } else {
            ashmem = nullptr;
            return UNKNOWN_ERROR;
        }
    }
}
