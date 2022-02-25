#include "hitcamera_handle_class_napi.h"

namespace OHOS::HITCamera {

    napi_value Capture(napi_env env, napi_callback_info info) {
        size_t argc = 2;
        napi_value args[2];
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

        uint32_t width = 0, height = 0;

        napi_valuetype valuetype1;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype1));
        NAPI_CALL(env, napi_get_value_uint32(env, args[0], &width));

        napi_valuetype valuetype2;
        NAPI_CALL(env, napi_typeof(env, args[1], &valuetype2));
        NAPI_CALL(env, napi_get_value_uint32(env, args[1], &height));

        auto handle = CameraManager::Instance().Capture(width, height);

        napi_value values[3];
        NAPI_CALL(env, napi_create_int32(env, handle.id, &values[0]));
        NAPI_CALL(env, napi_create_int32(env, handle.size, &values[1]));
        NAPI_CALL(env, napi_create_int64(env, handle.buffer, &values[2]));

        napi_value object = nullptr;
        NAPI_CALL(env, napi_new_instance(env, NPictureHandle::gConstructor, 3, values, &object));

        return object;
    }

    napi_value Release(napi_env env, napi_callback_info info) {
        size_t argc = 1;
        napi_value args[1];
        NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

        napi_valuetype valuetype;
        NAPI_CALL(env, napi_typeof(env, args[0], &valuetype));

        NPictureHandle* object;
        NAPI_CALL(env, napi_unwrap(env, args[0], reinterpret_cast<void**>(&object)));

        CameraManager::Instance().Release(object->mHandle);

        return nullptr;
    }
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    OHOS::HITCamera::NPictureHandle::Init(env, exports);

    napi_property_descriptor desc[] = {
            DECLARE_NAPI_STATIC_FUNCTION("capture", OHOS::HITCamera::Capture),
            DECLARE_NAPI_STATIC_FUNCTION("release", OHOS::HITCamera::Release),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    return exports;
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module Rpi4bCameraModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "hitcamerajs",
        .nm_priv = nullptr,
        .reserved = {nullptr}
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterModule(void) {
    napi_module_register(&Rpi4bCameraModule);
}
