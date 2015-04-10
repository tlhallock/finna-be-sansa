#ifndef IMAGE_H_
#define IMAGE_H_

#include "dense/kernel.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


template <class T>
class DenseImage
{
private:
	int width, height;
	ImageChannel<T> red;
	ImageChannel<T> green;
	ImageChannel<T> blue;
public:
    DenseImage(const cv::Mat& original)
	: width{ original.cols}
	, height{original.rows}
	, red  {width, height}
	, green{width, height}
	, blue {width, height}
	{
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				cv::Vec3b intensity = original.at<cv::Vec3b>(j, i);
				
				blue .data[i * height + j] = intensity.val[0] / 255.0;
				green.data[i * height + j] = intensity.val[1] / 255.0;
				red  .data[i * height + j] = intensity.val[2] / 255.0;
			}
		}
	}
	
    DenseImage(const DenseImage<T>& other, bool init)
	: width{ other.width}
	, height{other.height}
	, red  {width, height}
	, green{width, height}
	, blue {width, height}
	{
		if (!init)
		{
			return;
		}
		
		for (int i=0;i<width*height;i++)
		{
			red.data[i]   = other.red.data[i];
			green.data[i] = other.green.data[i];
			blue.data[i]  = other.blue.data[i];
		}
	}
	
    DenseImage(const DenseImage<T>& other) : DenseImage{other, true} {}
	
	void write(cv::Mat& original) const
	{
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				original.at<cv::Vec3b>(j, i)[0] = (uint8_t) (255 * (blue .data[i * height + j]));
				original.at<cv::Vec3b>(j, i)[1] = (uint8_t) (255 * (green.data[i * height + j]));
				original.at<cv::Vec3b>(j, i)[2] = (uint8_t) (255 * (red  .data[i * height + j]));
			}
		}
	}
	
	void gray()
	{
		for (int i=0;i<width * height;i++)
		{
			blue.data[i] = green.data[i] = red.data[i] = (blue.data[i] + green.data[i] + red.data[i]) / 3;
		}
	}

    void blueScale()
    {
        for (int i=0;i<width * height;i++)
        {
//            blue.data[i] = (blue.data[i] + green.data[i] + red.data[i]) / 3;
            green.data[i] = red.data[i] = 0;
        }
    }
	
	void apply(Kernel<T>& k, cv::Mat& output)
	{
        DenseImage<T> retVal{*this, false};
		
		apply(k, retVal);
		
		retVal.write(output);
	}
	
    void apply(Kernel<T>& k, DenseImage<T>& retVal)
	{
		k.apply(red, retVal.red);
		k.apply(green, retVal.green);
		k.apply(blue, retVal.blue);
	}
	
    void showEdges(DenseImage<T>& output)
	{
		if (output.width != width || output.height != height)
		{
			std::cerr << "Wrong size " << std::endl;
			exit(-1);
		}
		
        getLog() << "Blurring" << std::endl;
		Kernel<T> k1{5};
		k1.setGaussian(1.4);
        DenseImage<T> blurred{*this, false};
		apply(k1, blurred);
		
        getLog() << "Finding H lines" << std::endl;
		Kernel<T> k2{3};
		k2.setSobelH();
        DenseImage<T>& hlines = output;
		blurred.apply(k2, hlines);
		
		k2.setSobelV();
        DenseImage<T> vlines{*this, false};
        getLog() << "Finding V lines" << std::endl;
		blurred.apply(k2, vlines);
		
        getLog() << "Normalizing" << std::endl;
		hlines.blue.norm(vlines.blue);
		hlines.green.norm(vlines.green);
		hlines.red.norm(vlines.red);
		
        getLog() << "Graying" << std::endl;
		hlines.gray();
	}
};
	

#endif
