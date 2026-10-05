#include "CameraBackend.h"

#include <opencv2/videoio.hpp>

class OpenCVBackend final : public CameraBackend
{
public:
    bool open(int deviceId, std::string& error) override
    {
#ifdef _WIN32
        const bool ok = capture_.open(deviceId, cv::CAP_DSHOW);
#else
        const bool ok = capture_.open(deviceId);
#endif
        if (!ok) {
            error = "OpenCV could not open camera device " + std::to_string(deviceId) + ".";
            return false;
        }
        return true;
    }

    bool getImage(cv::Mat& image, std::string& error) override
    {
        if (!capture_.isOpened()) {
            error = "OpenCV camera is not open.";
            return false;
        }

        if (!capture_.read(image) || image.empty()) {
            error = "OpenCV failed to read a frame.";
            return false;
        }
        return true;
    }

    void close() override
    {
        if (capture_.isOpened()) {
            capture_.release();
        }
    }

    bool isOpened() const override
    {
        return capture_.isOpened();
    }

private:
    cv::VideoCapture capture_;
};

std::unique_ptr<CameraBackend> createOpenCVBackend()
{
    return std::make_unique<OpenCVBackend>();
}
