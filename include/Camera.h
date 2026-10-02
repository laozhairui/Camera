#ifndef CAMERA_H
#define CAMERA_H
#include <opencv2/opencv.hpp>
class Camera
{
public:
    Camera();
    ~Camera();

    bool open();
    bool start();
    bool getFrame(cv::Mat& image);
    bool stop();
    bool close();
private:
    void* m_handle;
    bool m_isGrabbing;
};
#endif