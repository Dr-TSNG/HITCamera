#include "take_picture.h"
#include "hitcamera_service_impl.h"

namespace OHOS::HITCamera {

    HITCameraService::HITCameraService(int32_t systemAbilityId, bool runOnCreate)
            : SystemAbility(systemAbilityId, runOnCreate) {}

    sptr<PictureHandle> HITCameraService::Capture() {
        return TakeOnePicture();
    }
}
