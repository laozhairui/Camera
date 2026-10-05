#include "Camera.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>

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
              << "Press S to save a frame.\n"
              << "Press R to start/stop video recording.\n"
              << "Press ESC or Q to quit.\n";

    cv::Mat frame;
    const std::string windowName = "Camera";

    cv::VideoWriter videoWriter;
    bool recording = false;

    while (true) {
        if (!camera.getImage(frame)) {
            std::cerr << "Image acquisition failed: "
                      << camera.lastError() << '\n';
            break;
        }

        cv::imshow(windowName, frame);

        if (recording) {
            videoWriter.write(frame);
        }

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

        if (key == 'r' || key == 'R') {
            if (!recording) {
                const std::string filename = "capture.mp4";
                const int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');

                videoWriter.open(
                    filename,
                    fourcc,
                    30.0,
                    cv::Size(frame.cols, frame.rows)
                );

                if (!videoWriter.isOpened()) {
                    std::cerr << "Failed to start video recording.\n";
                } else {
                    recording = true;
                    std::cout << "Recording started: "
                              << filename << '\n';
                }
            } else {
                videoWriter.release();
                recording = false;

                std::cout << "Recording stopped.\n";
                std::cout << "Video saved: capture.mp4\n";
            }
        }
    }

    if (recording) {
        videoWriter.release();
        recording = false;
        std::cout << "Recording stopped.\n";
        std::cout << "Video saved: capture.mp4\n";
    }

    camera.close();
    cv::destroyAllWindows();

    std::cout << "Camera closed.\n";
    return 0;
}

