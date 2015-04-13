
#include "subimg/roi.h"
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>


#define MAX_DELTA 200
#define MAX_FAIL 4


new_offset::new_offset(int x_, int y_)
        : x{x_}
        , y{y_} {}


#if 0
    roi::roi()
        : w{-1}
        , h{-1}
        , px{-1}
        , py{-1}
        , latestImage{}
        , failCount{0} {}
#endif

roi::roi(int x_, int y_, int w_, int h_, const cv::Mat& original)
        : w{w_}
        , h{h_}
        , px{x_}
        , py{y_}
        , latestImage{original, cv::Rect{px, py, w, h}}
        , failCount{0} {}

bool roi::found(int imageId,
                const cv::Mat& img,
                int nx, int ny)
{
    if (std::abs(nx - px) > MAX_DELTA || std::abs(ny - py) > MAX_DELTA)
    {
        std::cerr << "Unable to find image!!" << std::endl;
        return ++failCount >= MAX_FAIL;
    }
    latestImage = cv::Mat{img, cv::Rect(nx, ny, w, h)};
    history.insert(std::pair<int, new_offset>{imageId, new_offset{nx, ny}});
    failCount = 0;
    return false;
}

const cv::Mat& roi::getSubImage() const
{
    return latestImage;
}


void roi::saveBox(cv::Mat& m,
                  int imageId,
                  const std::string& name,
                  const cv::Scalar& c) const
{
    cv::Mat myCopy = m.clone();

#if 0
    for (int i=0;i<h;i++)
        for (int j=0;j<w;j++)
            myCopy.at<cv::Vec3b>(py + i, px + j) = c;
#endif

    cv::line(myCopy, cv::Point{px, py}         , cv::Point{px + w, py}    , c, 2);
    cv::line(myCopy, cv::Point{px + w, py}     , cv::Point{px + w, py + h}, c, 2);
    cv::line(myCopy, cv::Point{px + w, py + h} , cv::Point{px, py + h}    , c, 2);
    cv::line(myCopy, cv::Point{px, py + h}     , cv::Point{px, py}        , c, 2);

    std::stringstream fname;
    fname << "imgs/img-" << std::setfill('0') << std::setw(5) << imageId << "-" << name << ".png";
    cv::imwrite(fname.str(), myCopy);

#if 0
    cv::namedWindow(name,  cv::WINDOW_AUTOSIZE);
    cv::imshow(name, myCopy);
#endif
}
