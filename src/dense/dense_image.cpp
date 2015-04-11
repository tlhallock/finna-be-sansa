
#include "dense/dense_image.h"

DenseImage::DenseImage(const cv::Mat& original)
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

DenseImage::DenseImage(const DenseImage<T>& other, bool init)
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

DenseImage::DenseImage(const DenseImage<T>& other) : DenseImage{other, true} {}

void DenseImage::write(cv::Mat& original) const
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

void DenseImage::gray()
{
    for (int i=0;i<width * height;i++)
    {
        blue.data[i] = green.data[i] = red.data[i] = (blue.data[i] + green.data[i] + red.data[i]) / 3;
    }
}

void DenseImage::blueScale()
{
    for (int i=0;i<width * height;i++)
    {
//            blue.data[i] = (blue.data[i] + green.data[i] + red.data[i]) / 3;
        green.data[i] = red.data[i] = 0;
    }
}

void DenseImage::apply(Kernel<T>& k, cv::Mat& output)
{
    DenseImage<T> retVal{*this, false};

    apply(k, retVal);

    retVal.write(output);
}

void DenseImage::apply(Kernel<T>& k, DenseImage<T>& retVal)
{
    k.apply(red, retVal.red);
    k.apply(green, retVal.green);
    k.apply(blue, retVal.blue);
}

void DenseImage::showEdges(DenseImage<T>& output)
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



double getDistance(const DenseImage<T>& i1,
                   int x1, int y1,
                   const DenseImage<T>& i2,
                   int x2, int y2,
                   int w, int h)
{
    double retVal = 0.0;
    for (int i=0;i<w;i++)
    {
        for (int j=0;j<h;j++)
        {
            double diff;
            diff = i1.blue.data[i1.blue.width * (x1 + i) + y1 + j] - i2.blue.data[i2.blue.width * (x2 + i) + j + y2];
            retVal += diff * diff;

            diff = i1.green.data[i1.green.width * (x1 + i) + y1 + j] - i2.green.data[i2.green.width * (x2 + i) + j + y2];
            retVal += diff * diff;

            diff = i1.red.data[i1.red.width * (x1 + i) + y1 + j] - i2.red.data[i2.red.width * (x2 + i) + j + y2];
            retVal += diff * diff;
        }
    }
    return retVal;
}
