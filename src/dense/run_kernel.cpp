#include "dense/dense_image.h"


void blueScale()
{
    cv::Mat bimage = cv::imread("/work/drone/ransac/sunny.jpg", CV_LOAD_IMAGE_COLOR);
    DenseImage<double> sunny{bimage};
    sunny.blueScale();
    sunny.write(bimage);
    cv::imwrite("/work/drone/ransac/blue_sunny2.png", bimage);

}

int main2( int argc, char** argv )
{
	if( argc != 2)
	{
        getLog() <<" Usage: display_image ImageToLoadAndDisplay" << std::endl;
		return -1;
	}

	cv::Mat image = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);

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

	cv::waitKey(0);                                          // Wait for a keystroke in the window
	return 0;
}

