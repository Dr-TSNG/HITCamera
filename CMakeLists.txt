cmake_minimum_required(VERSION 3.16)
project(HITCamera)

set(CMAKE_CXX_STANDARD 17)

include_directories("ohos/ipc")
include_directories("ohos/utils")

include_directories("base")
include_directories("client")
include_directories("service")

add_library(HITCamera service/ihitcamera_service.cpp)
