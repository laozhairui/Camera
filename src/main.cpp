
#include <iostream>
#include "Camera.h"
#include <opencv2/highgui.hpp>

int main()
{
    Camera camera;

    if (!camera.open())
    {
        std::cerr << "Failed to open camera!" << std::endl;
        return -1;
    }

    std::cout << "Camera opened successfully!" << std::endl;
    std::cout << "Press ESC to exit." << std::endl;

    cv::Mat frame;

    while (true)
    {
        if (!camera.getImage(frame))
        {
            std::cerr << "Failed to read frame!" << std::endl;
            break;
        }

        cv::imshow("Camera", frame);

        if (cv::waitKey(1) == 27)
        {
            break;
        }
    }

    camera.close();
    cv::destroyAllWindows();

    return 0;
}

