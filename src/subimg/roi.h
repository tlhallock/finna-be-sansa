#ifndef ROI_H
#define ROI_H

#include <opencv2/core/core.hpp>

class new_offset
{
public:
    int x, y;
    new_offset(int x_, int y_);
};

class roi
{
private:
    int w, h;
    std::map<int, new_offset> history;
    int px, py;
    cv::Mat latestImage;
    int failCount;

public:
    roi(int x_, int y_, int w_, int h_, const cv::Mat& original);

    bool found(int imageId,
               const cv::Mat& img,
               int nx, int ny);

    const cv::Mat& getSubImage() const;


    void saveBox(const cv::Mat& m,
             int imageId,
             const std::string& name,
             const cv::Scalar& c) const;
};


#endif // ROI_H
