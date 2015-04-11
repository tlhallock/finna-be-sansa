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
    DenseImage(const cv::Mat& original);
    DenseImage(const DenseImage<T>& other, bool init);
    DenseImage(const DenseImage<T>& other);
	
    void write(cv::Mat& original) const;

    void gray();
    void blueScale();
	
    void apply(Kernel<T>& k, cv::Mat& output);
    void apply(Kernel<T>& k, DenseImage<T>& retVal);

    void showEdges(DenseImage<T>& output);


    friend double getDistance(const DenseImage& i1,
                       int x1, int y1,
                       const DenseImage& i2,
                       int x2, int y2,
                       int w, int h);
};


#endif
