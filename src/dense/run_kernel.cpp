#include "dense/dense_image.h"




#define TEST_SIZE 50





int main2(int argc, char** argv)
{
    cv::Mat image1 = cv::imread("/work/drone/ransac/image1.png", CV_LOAD_IMAGE_COLOR);
    cv::Mat image2 = cv::imread("/work/drone/ransac/image2.png", CV_LOAD_IMAGE_COLOR);


    DenseImage<double> im1{image1};
    DenseImage<double> im2{image2};

#if 0

    for (int x1 = 0; x1 < i1.)

    friend double getDistance(const DenseImage& i1,
                       int x1, int y1,
                       const DenseImage& i2,
                       int x2, int y2,
                       int w, int h);


    getDistance()
    {

    }


#endif
    return 0;
}



void blueScale()
{
    cv::Mat bimage = cv::imread("/work/drone/ransac/sunny.jpg", CV_LOAD_IMAGE_COLOR);
    DenseImage<double> sunny{bimage};
    sunny.blueScale();
    sunny.write(bimage);
    cv::imwrite("/work/drone/ransac/blue_sunny2.png", bimage);

}

int getEdges(const char *filename)
{
    initLogging();
#if 0
    if( argc != 2)
    {
        getLog() <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
        return -1;
    }
#endif

    cv::Mat image = cv::imread(filename, CV_LOAD_IMAGE_COLOR);

    if (!image.data)
    {
        getLog() << "Could not open or find the image" << std::endl;
        return -1;
    }

    DenseImage<double> im{image};
    DenseImage<double> edges{im, false};
    im.showEdges(edges);

    cv::Mat output = image.clone();
    edges.write(output);

    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Display window", image );                   // Show our image inside it.

    cv::namedWindow( "Gaussian window", cv::WINDOW_AUTOSIZE );// Create a window for display.
    cv::imshow( "Gaussian window", output);                   // Show our image inside it.

//    cv::waitKey(0);                                          // Wait for a keystroke in the window

    return 0;
}
