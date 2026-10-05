#ifndef CAMERA_BACKEND_H
#define CAMERA_BACKEND_H

#include <string>
#include <opencv2/core.hpp>

class CameraBackend
{
public:
    virtual ~CameraBackend() = default;
    virtual bool open(int deviceId, std::string& error) = 0;
    virtual bool getImage(cv::Mat& image, std::string& error) = 0;
    virtual void close() = 0;
    virtual bool isOpened() const = 0;
};

#endif // CAMERA_BACKEND_H
