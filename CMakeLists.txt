cmake_minimum_required(VERSION 3.22)
project(Camera LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Select OPENCV for a normal webcam, or MVS for a Hikvision camera.
set(CAMERA_BACKEND "OPENCV" CACHE STRING "Camera backend: OPENCV or MVS")
set_property(CACHE CAMERA_BACKEND PROPERTY STRINGS OPENCV MVS)

find_package(OpenCV REQUIRED COMPONENTS core videoio highgui imgcodecs)

add_executable(Camera
    src/main.cpp
    src/Camera.cpp
)

target_include_directories(Camera PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(Camera PRIVATE ${OpenCV_LIBS})

if(CAMERA_BACKEND STREQUAL "OPENCV")
    target_sources(Camera PRIVATE src/OpenCVBackend.cpp)
    target_compile_definitions(Camera PRIVATE CAMERA_BACKEND_OPENCV=1)

elseif(CAMERA_BACKEND STREQUAL "MVS")
    set(MVS_ROOT "/opt/MVS" CACHE PATH "Hikvision MVS SDK root directory")
    find_path(MVS_INCLUDE_DIR
        NAMES MvCameraControl.h
        PATHS "${MVS_ROOT}/include" "${MVS_ROOT}/Development/Includes"
        REQUIRED
    )
    find_library(MVS_LIBRARY
        NAMES MvCameraControl
        PATHS
            "${MVS_ROOT}/lib/64"
            "${MVS_ROOT}/lib"
            "${MVS_ROOT}/Development/Libraries"
            "/opt/MVS_Runtime/MVS/lib/64"
        REQUIRED
    )

    target_sources(Camera PRIVATE src/MVSBackend.cpp)
    target_include_directories(Camera PRIVATE ${MVS_INCLUDE_DIR})
    target_link_libraries(Camera PRIVATE ${MVS_LIBRARY})
    target_compile_definitions(Camera PRIVATE CAMERA_BACKEND_MVS=1)

    get_filename_component(MVS_LIBRARY_DIR "${MVS_LIBRARY}" DIRECTORY)
    set_target_properties(Camera PROPERTIES
        BUILD_RPATH "${MVS_LIBRARY_DIR}"
        INSTALL_RPATH "${MVS_LIBRARY_DIR}"
    )
else()
    message(FATAL_ERROR "Unsupported CAMERA_BACKEND='${CAMERA_BACKEND}'. Use OPENCV or MVS.")
endif()

message(STATUS "Camera backend: ${CAMERA_BACKEND}")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
