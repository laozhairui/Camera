
#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

class Camera
{
public:
    Camera();
    ~Camera();

    bool open(int deviceId = 0);
    void close();

    bool isOpened() const;

    bool getImage(cv::Mat& image);

private:
    cv::VideoCapture cap_;
};

#endif // CAMERA_H

