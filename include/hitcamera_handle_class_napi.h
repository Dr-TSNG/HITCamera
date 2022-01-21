#pragma once

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "hitcamera_manager.h"

namespace OHOS::HITCamera {

    class NPictureHandle {
    public:
        static inline napi_value gConstructor;

        explicit NPictureHandle(PictureHandle handle);

        static void Init(napi_env env, napi_value exports);

        static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);

        friend napi_value Capture(napi_env env, napi_callback_info info);

        friend napi_value Release(napi_env env, napi_callback_info info);

    private:
        ~NPictureHandle();

        static napi_value Constructor(napi_env env, napi_callback_info info);
        static napi_value GetId(napi_env env, napi_callback_info info);
        static napi_value GetSize(napi_env env, napi_callback_info info);
        static napi_value GetBuffer(napi_env env, napi_callback_info info);

        napi_env mEnv;
        napi_ref mWrapper;
        PictureHandle mHandle;
    };
}
