cmake_minimum_required(VERSION 3.22)

project(Camera)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "/usr/local/lib/cmake/opencv4")

find_package(OpenCV REQUIRED)

set(MVS_INCLUDE_DIR "/opt/MVS/include")

set(MVS_LIBRARY_DIR "/opt/MVS_Runtime/MVS/lib/64")

find_library(MVS_CAMERA_LIBRARY
    NAMES MvCameraControl
    PATHS ${MVS_LIBRARY_DIR}
    NO_DEFAULT_PATH
)

if(NOT MVS_CAMERA_LIBRARY)
    message(FATAL_ERROR "MVS library not found!")
endif()

add_executable(Camera
    src/main.cpp
    src/Camera.cpp
)

target_include_directories(Camera PRIVATE
    include
    ${MVS_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(Camera PRIVATE
    ${MVS_CAMERA_LIBRARY}
    ${OpenCV_LIBS}
)