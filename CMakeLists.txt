cmake_minimum_required(VERSION 3.16)
project(HITCamera)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        "ohos"
        "ohos/ipc"
        "ohos/node"
        "ohos/sa"
        "ohos/samgr"
        "ohos/utils"
)

include_directories(
        services/include
        services/server/include
        interfaces/innerkits
        interfaces/kits/js/include
        interfaces/kits/native
)

add_library(HITCamera
        services/server/hitcamera_service_impl.cpp
        services/server/hitcamera_service_stub.cpp
        services/server/take_picture.cpp
        frameworks/kitsimpl/js/camera_handle_class_napi.cpp
        frameworks/kitsimpl/js/hit_camera_napi.cpp
        frameworks/kitsimpl/native/hitcamera_manager.cpp
        frameworks/kitsimpl/native/hitcamera_service_proxy.cpp
        frameworks/kitsimpl/native/circular_buffer.cpp
        frameworks/kitsimpl/native/udp_piece.cpp
        test/test.cpp
        test/native_test.cpp
        test/udp_test.cpp
        udp/circular_buffer.c
        udp/udp-piece.c
        udp/udp-piece-server.c
)
