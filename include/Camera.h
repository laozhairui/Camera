#ifndef CAMERA_H
#define CAMERA_H

#include <memory>
#include <string>
#include <opencv2/core.hpp>

class CameraBackend;

class Camera
{
public:
    Camera();
    ~Camera();

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;

    // Open the default camera device. deviceId is used by the OpenCV backend.
    // For the MVS backend, deviceId selects an item from the enumerated device list.
    bool open(int deviceId = 0);

    // Get one frame. Returns false if the camera is closed or acquisition fails.
    bool getImage(cv::Mat& image);

    void close();
    bool isOpened() const;

    // Human-readable diagnostic message for the most recent operation.
    std::string lastError() const;

private:
    std::unique_ptr<CameraBackend> backend_;
    std::string lastError_;
};

#endif // CAMERA_H
