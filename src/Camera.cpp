
#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
    close();
}

bool Camera::open(int deviceId)
{
    return cap_.open(deviceId, cv::CAP_DSHOW);
}

void Camera::close()
{
    if (cap_.isOpened())
    {
        cap_.release();
    }
}

bool Camera::isOpened() const
{
    return cap_.isOpened();
}

bool Camera::getImage(cv::Mat& image)
{
    if (!cap_.isOpened())
    {
        return false;
    }

    return cap_.read(image);
}

