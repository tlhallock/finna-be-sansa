#include <iostream>
#include <iomanip>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/video/video.hpp>

#define STEP_SIZE 50
#define NUM_IMAGES 5
#define START_IMAGE 10000
#define VIS 1

int main25(int argc, char **argv)
{
//    cv::VideoCapture cap {"/media/rever/18D2394BD2392E7E/GOPR1596.MP4"};
    cv::VideoCapture cap {"/media/thallock/8765-4321/DCIM/100GOPRO/GOPR1596.MP4"};
    cv::Mat mat;

    cv::namedWindow("video", cv::WINDOW_AUTOSIZE);

    for (int i=0;i<NUM_IMAGES;i++)
    {
        cap.set(CV_CAP_PROP_POS_FRAMES, START_IMAGE + i * STEP_SIZE);
        cap.read(mat);

#if VIS
        cv::imshow("video", mat);
        cv::waitKey(1);
#endif

        std::stringstream fname;
        fname << "imgs/img-" << std::setfill('0') << std::setw(5) << i << ".png";
        cv::imwrite(fname.str(), mat);
    }

    return 0;
}





