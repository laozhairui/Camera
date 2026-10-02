#include <iostream>
#include <opencv2/opencv.hpp>
#include "Camera.h"
int main()
{
    std::cout<<"Camera Application Start"<<std::endl;
    Camera camera;

    if (!camera.open())
    {
        std::cout << "Camera open failed!" << std::endl;
        return -1;
    }

    std::cout << "Camera opened successfully!" << std::endl;

    if (!camera.start())
    {
        std::cout << "Camera start failed!" << std::endl;
        return -1;
    }

    std::cout << "Camera started successfully!" << std::endl;

    cv::Mat image;

    if (camera.getFrame(image))
    {
        std::cout << "Frame acquired successfully!" << std::endl;
        cv::imshow("Camera", image);
        cv::imwrite("image.png", image);
        cv::waitKey(0);
        std::cout << "Image size: "
                  << image.cols
                  << " x "
                  << image.rows
                  << std::endl;
    }
    else
    {
        std::cout << "Frame acquisition failed!" << std::endl;
    }
    std::cout<<"Camera Application End"<<std::endl;

    return 0;
}