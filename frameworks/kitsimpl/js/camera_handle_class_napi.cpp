#include <cassert>
#include "hitcamera_manager.h"
#include "hitcamera_handle_class_napi.h"

namespace OHOS::HITCamera {

    NPictureHandle::NPictureHandle(PictureHandle handle)
            : mEnv(nullptr), mWrapper(nullptr), mHandle(handle) {}

    NPictureHandle::~NPictureHandle() {
        napi_delete_reference(mEnv, mWrapper);
    }

    void NPictureHandle::Destructor(napi_env env,
                                    void* nativeObject,
                                    void* finalize_hint) {
        reinterpret_cast<NPictureHandle*>(nativeObject)->~NPictureHandle();
    }

    void NPictureHandle::Init(napi_env env, napi_value exports) {
        napi_status status;
        napi_property_descriptor properties[] = {
                DECLARE_NAPI_GETTER("id", GetId),
                DECLARE_NAPI_GETTER("size", GetSize),
                DECLARE_NAPI_GETTER("buffer", GetBuffer)
        };

        status = napi_define_class(
                env, "NPictureHandle",
                NAPI_AUTO_LENGTH, Constructor, nullptr, 3, properties, &gConstructor);
        assert(status == napi_ok);
        napi_set_named_property(env, exports, "NPictureHandle", gConstructor);
    }

    napi_value NPictureHandle::Constructor(napi_env env, napi_callback_info info) {
        napi_value target;
        NAPI_CALL(env, napi_get_new_target(env, info, &target));
        bool is_constructor = target != nullptr;

        if (is_constructor) {
            // Invoked as constructor: `new NPictureHandle(...)`
            size_t argc = 3;
            napi_value args[3];
            napi_value jsthis;
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &jsthis, nullptr));

            PictureHandle p{};

            napi_valuetype valuetype0;
            NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
            NAPI_CALL(env, napi_get_value_int32(env, args[0], &p.id));

            napi_valuetype valuetype1;
            NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
            NAPI_CALL(env, napi_get_value_int32(env, args[1], &p.size));

            napi_valuetype valuetype2;
            NAPI_CALL(env, napi_typeof(env, args[2], &valuetype2));
            NAPI_CALL(env, napi_get_value_int64(env, args[2], &p.buffer));

            auto* obj = new NPictureHandle(p);
            obj->mEnv = env;
            NAPI_CALL(env, napi_wrap(
                    env, jsthis,
                    reinterpret_cast<void*>(obj),
                    Destructor,
                    nullptr,
                    &obj->mWrapper
            ));

            return jsthis;
        } else {
            // Invoked as plain function `NPictureHandle(...)`, turn into construct call.
            size_t argc_ = 3;
            napi_value args[3];
            NAPI_CALL(env, napi_get_cb_info(env, info, &argc_, args, nullptr, nullptr));

            const size_t argc = 3;
            napi_value argv[argc] = {args[0], args[1], args[2]};

            napi_value instance;
            NAPI_CALL(env, napi_new_instance(env, gConstructor, argc, argv, &instance));

            return instance;
        }
    }

    napi_value NPictureHandle::GetId(napi_env env, napi_callback_info info) {
        napi_value jsthis;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

        NPictureHandle* obj;
        NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

        napi_value num;
        NAPI_CALL(env, napi_create_int32(env, obj->mHandle.id, &num));

        return num;
    }

    napi_value NPictureHandle::GetSize(napi_env env, napi_callback_info info) {
        napi_value jsthis;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

        NPictureHandle* obj;
        NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

        napi_value num;
        NAPI_CALL(env, napi_create_int32(env, obj->mHandle.size, &num));

        return num;
    }

    napi_value NPictureHandle::GetBuffer(napi_env env, napi_callback_info info) {
        napi_value jsthis;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &jsthis, nullptr));

        NPictureHandle* obj;
        NAPI_CALL(env, napi_unwrap(env, jsthis, reinterpret_cast<void**>(&obj)));

        napi_value num;
        NAPI_CALL(env, napi_create_int64(env, obj->mHandle.buffer, &num));

        return num;
    }
}
