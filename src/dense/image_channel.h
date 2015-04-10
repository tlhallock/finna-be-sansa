

#ifndef IMAGE_CHANNEL_H_
#define IMAGE_CHANNEL_H_

#include <cmath>

template <class T>
class ImageChannel
{
public:
	int width;
	int height;
	T* data;
	
	ImageChannel(int w, int h)
		: width{w}
		, height{h}
		, data{new T[w * h]}
	{
		//getLogger() << width << ", " << height << ": " << sizeof (T) * width * height << ", " << width * height << std::endl;
	}
	
	
	void norm(const ImageChannel& other)
	{
		for (int i=0;i<width*height;i++)
		{
			data[i] = sqrt(data[i] * data[i] + other.data[i] * other.data[i]);
		}
	}
	
	virtual ~ImageChannel() { delete[] data; }
};





#endif
