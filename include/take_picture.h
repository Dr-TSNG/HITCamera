#pragma once

#include <variant>
#include <refbase.h>
#include "ihitcamera_service.h"

namespace OHOS::HITCamera {

    std::variant<sptr<Ashmem>, int> TakeOnePicture(uint32_t width, uint32_t height);
}
