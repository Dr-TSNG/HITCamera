#include "take_picture.h"
#include "hitcamera_service_impl.h"

namespace OHOS::HITCamera {

    HITCameraService::HITCameraService(int32_t systemAbilityId, bool runOnCreate)
            : SystemAbility(systemAbilityId, runOnCreate) {}

    int HITCameraService::Capture(sptr<PictureHandle>& handle) {
        handle = TakeOnePicture();
        return NO_ERROR;
    }
}
