#include "Camera.h"
#include "CameraBackend.h"

#include <memory>

#ifdef CAMERA_BACKEND_OPENCV
std::unique_ptr<CameraBackend> createOpenCVBackend();
#elif defined(CAMERA_BACKEND_MVS)
std::unique_ptr<CameraBackend> createMVSBackend();
#endif

Camera::Camera() = default;

Camera::~Camera()
{
    close();
}

bool Camera::open(int deviceId)
{
    close();
    lastError_.clear();

#ifdef CAMERA_BACKEND_OPENCV
    backend_ = createOpenCVBackend();
#elif defined(CAMERA_BACKEND_MVS)
    backend_ = createMVSBackend();
#else
    lastError_ = "No camera backend was selected at build time.";
    return false;
#endif

    if (!backend_) {
        lastError_ = "Failed to create camera backend.";
        return false;
    }

    if (!backend_->open(deviceId, lastError_)) {
        backend_.reset();
        return false;
    }

    return true;
}

bool Camera::getImage(cv::Mat& image)
{
    if (!isOpened()) {
        lastError_ = "Camera is not open.";
        return false;
    }

    if (!backend_->getImage(image, lastError_)) {
        return false;
    }

    return !image.empty();
}

void Camera::close()
{
    if (backend_) {
        backend_->close();
        backend_.reset();
    }
}

bool Camera::isOpened() const
{
    return backend_ && backend_->isOpened();
}

std::string Camera::lastError() const
{
    return lastError_;
}
