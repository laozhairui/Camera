#include "Camera.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <string>

int main()
{
    Camera camera;

    if (!camera.open(0)) {
        std::cerr << "Failed to open camera: " << camera.lastError() << '\n';
        return 1;
    }

    std::cout << "Camera opened successfully.\n"
              << "Press S to save a frame; press ESC or Q to quit.\n";

    cv::Mat frame;
    const std::string windowName = "Camera";

    while (true) {
        if (!camera.getImage(frame)) {
            std::cerr << "Image acquisition failed: " << camera.lastError() << '\n';
            break;
        }

        cv::imshow(windowName, frame);
        const int key = cv::waitKey(1);

        if (key == 27 || key == 'q' || key == 'Q') {
            break;
        }

        if (key == 's' || key == 'S') {
            const std::string filename = "capture.jpg";
            if (cv::imwrite(filename, frame)) {
                std::cout << "Saved image: " << filename << '\n';
            } else {
                std::cerr << "Failed to save image.\n";
            }
        }
    }

    camera.close();
    cv::destroyAllWindows();
    std::cout << "Camera closed.\n";
    return 0;
}
