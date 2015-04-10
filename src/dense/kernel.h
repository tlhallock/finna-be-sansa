

#ifndef KERNEL_H_
#define KERNEL_H_

#include "dense/image_channel.h"
#include "util/logger.h"
#include <iostream>


#define ABS(x) ((x)<0?-(x):(x))
#define SQ(x) ((x)*(x))

#define BOUND(x,y,z)				\
(			 			\
	((x)<(y))? 				\
	( 					\
		((y)<(z))?			\
			(y)			\
		:				\
			(z)			\
	)					\
	:					\
		(x) 				\
)

#define PI 3.1415926535897932384626433832795

template <class T>
class Kernel
{
private:
	int width, height;
	T *data;
public:
	Kernel(int w, int h) : width{w}, height{h}, data{new T[width * height]} {}
	Kernel(int s) : Kernel{s, s} {}
	virtual ~Kernel() { delete[] data; }
	
	void setAverage()
	{
		for (int i=0;i<width*height;i++)
		{
			data[i] = 1;
		}
		normalize();
	}
	
	void setGaussian(double sigma)
	{
		double ss = sigma*sigma;
		if (width != height || (width % 2) == 0)
		{
            getLog() << "Not implemented" << std::endl;
			exit(-1);
		}
		int k = width / 2;
		T den = 1 / (2 * PI * ss);
		
		for (int i=0;i<width;i++)
		{
			for (int j=0;j<height;j++)
			{
				data[i * height + j] = den * exp(-(SQ(i-k-1) + SQ(j-k-1)) / (2 * ss));
			}
		}
		
//		normalize();
	}
	
	void setSobelH()
	{
		if (width != 3)
		{
			std::cerr << "Not implemented" << std::endl;
			exit(-1);
		}
		
		data[0 * 3 + 0] = -1; data[0 * 3 + 1] =  0; data[0 * 3 + 2] =  1;
		data[1 * 3 + 0] = -2; data[1 * 3 + 1] =  0; data[1 * 3 + 2] =  2;
		data[2 * 3 + 0] = -1; data[2 * 3 + 1] =  0; data[2 * 3 + 2] =  1;
	}
	
	void setSobelV()
	{
		if (width != 3)
		{
			std::cerr << "Not implemented" << std::endl;
			exit(-1);
		}
		
		data[0 * 3 + 0] = -1; data[0 * 3 + 1] = -2; data[0 * 3 + 2] = -1;
		data[1 * 3 + 0] =  0; data[1 * 3 + 1] =  0; data[1 * 3 + 2] =  0;
		data[2 * 3 + 0] =  1; data[2 * 3 + 1] =  2; data[2 * 3 + 2] =  1;
	}
	
	void apply(ImageChannel<T>& input, ImageChannel<T>& output) const
	{	
		if (input.width != output.width || input.height != output.height)
		{
			std::cerr << "This is not implemented!" << std::endl;
			exit(-1);
		}
		
		for (int ii = 0; ii<output.width; ++ii)
		{
			for (int ij=0; ij<output.height; ++ij)
			{
				T value = 0;
				for (int i=0; i<width; i++)
				{
					for (int j=0; j<height; j++)
					{
						int inputI = ii - width / 2 + i;
						int inputJ = ij - height / 2 + j;
						
						inputI = BOUND(0, inputI, input.width -1);
						inputJ = BOUND(0, inputJ, input.height-1);
						
						value += data[i * height + j] * input.data[inputI * input.height + inputJ];
					}
				}
				output.data[ii * output.height + ij] = value;
			}
		}
	}
	
	void normalize()
	{
		T retVal = 0;
		for (int i=0;i<width*height;i++)
		{
			retVal += ABS(data[i]);
		}
		for (int i=0;i<width*height;i++)
		{
			data[i] /= retVal;
		}
		return retVal;
	}
};










#endif


