cmake_minimum_required(VERSION 3.16)
project(HITCamera)

set(CMAKE_CXX_STANDARD 17)

include_directories("ohos")
include_directories("ohos/ipc")
include_directories("ohos/node")
include_directories("ohos/sa")
include_directories("ohos/samgr")
include_directories("ohos/utils")

include_directories("include")

add_library(HITCamera
        server/take_picture.cpp
        server/hitcamera_service_stub.cpp
        server/hitcamera_service_impl.cpp
        client/hitcamera_service_proxy.cpp
        client/hitcamera_manager.cpp
        src/camera_handle_class_napi.cpp
        src/rpi4b_camera_napi.cpp)
