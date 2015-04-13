#ifndef ROI_H
#define ROI_H

#include <opencv2/core/core.hpp>

#define MAX_DELTA 200
#define MAX_FAIL 4


class new_offset
{
public:
    int x, y;
    new_offset(int x_, int y_)
        : x{x_}
        , y{y_} {}
};

class roi
{
public:
    int w, h;
    std::map<int, new_offset> history;
    int px, py;
    cv::Mat latestImage;
    int failCount;

    roi(int x_, int y_, int w_, int h_, const cv::Mat& original)
        : w{w_}
        , h{h_}
        , px{x_}
        , py{y_}
        , latestImage{m, cv::Rect{px, py, w, h}}
        , failCount{0}
    {

    }

    bool found(int imageId,
               const cv::Mat& img,
               int nx, int ny)
    {
        if (std::abs(nx - px) > x || std::abs(ny - py) > MAX_DELTA)
        {
            return ++failCount >= MAX_FAIL;
        }
        latestImage = cv::Mat{img, cv::Rect(x, y, w, h)};
        history.insert(std::pair<int, new_offset>{imageId, nx, ny});
        failCount = 0;
        return false;
    }
};


#endif // ROI_H
