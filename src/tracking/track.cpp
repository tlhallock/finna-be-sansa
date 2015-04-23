
#include "tracking/trackingcontext.h"


#define IMG1 "/work/ransac/finna-be-sansa/imgs/img-00000.png"
#define IMG2 "/work/ransac/finna-be-sansa/imgs/img-00002.png"

int main(int argc, char **argv)
{
    cv::namedWindow("Original window", cv::WINDOW_AUTOSIZE);// Create a window for display.
    cv::namedWindow("Next window", cv::WINDOW_AUTOSIZE);// Create a window for display.

    cv::Mat i1 = cv::imread(IMG1);
    cv::Mat i2 = cv::imread(IMG2);

    ImgData im1 {i1};
    ImgData im2 {i2};
	
    TrackedObject obj{i1, 620, 483, 30, 30};

    Transform guess;
    guess.translationX = 30;
    guess.translationY = -10;
	
    Transform t = seek(im1, im2, obj, guess,


                       i1, i2);
	
    cv::Scalar c(0, 0, 0);
    t.visualize(i1, i2, obj, c);
	
    // show the two images...
    cv::imshow("Original window", i1);
    cv::imshow("Next window", i2);
	
    cv::waitKey();
	
	return 0;
}
