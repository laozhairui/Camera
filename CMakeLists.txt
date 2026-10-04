
cmake_minimum_required(VERSION 3.22)

project(Camera)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenCV_DIR "C:/opencv_build")

find_package(OpenCV REQUIRED)

add_executable(Camera
    src/main.cpp
    src/Camera.cpp
)

target_include_directories(Camera
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(Camera
    PRIVATE
    C:/opencv_source/include
    C:/opencv_build
    C:/opencv_source/modules/core/include
    C:/opencv_source/modules/videoio/include
    C:/opencv_source/modules/highgui/include
    C:/opencv_source/modules/imgcodecs/include
)

target_link_libraries(Camera
    PRIVATE
    opencv_core
    opencv_videoio
    opencv_highgui
    opencv_imgcodecs
)

