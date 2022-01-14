#include "hitcamera_service_impl.h"

namespace OHOS {

    HITCameraService::HITCameraService(int32_t systemAbilityId, bool runOnCreate)
            : SystemAbility(systemAbilityId, runOnCreate) {}

    int32_t HITCameraService::Capture(PictureHandle& handle) {
        // TODO
        return 0;
    }


}
