#ifndef TRACKINGCONTEXT_H
#define TRACKINGCONTEXT_H

#include <vector>

class ImgData
{
private:
    double *red, *green, *blue;
    int width, height;

public:
    ImgData();
    ~ImgData();

    ImgData(const ImgData& other) = delete;
    ImgData(ImgData& other) = delete;
    ImgData& operator=(const ImgData& other) = delete;
    ImgData& operator=(ImgData& other) = delete;

    void setSubImg(const cv::Mat& larger, int x, int y, int w, int h);
    void setData(const cv::Mat& mat);

private:
    void ensureSize(int w, int h);
};


class TrackedObject
{
    // Right now just a rectangle
    int x, y, w, h;
    ImgData current;
public:
    TrackedObject();
};


class Transform
{
private:
    int cx, cy;
    double rotation;
    double expansion;
    double translationX, translationY;
public:
    Transform();

    void seek(const ImgData& prev, const ImgData& next, const TrackedObject& obj);
    double currentCost(const ImgData& prev, const ImgData& next, const TrackedObject& obj);

    void visualize(cv::Mat& prev, cv::Mat& next, cv::Scalar& color);

    void inline map(const int ox, const int oy, int& nx, int& ny) const
    {
        const double dx = ox - cx;
        const double dy = oy - cy;
        double r = sqrt(dx*dx+dy*dy);
        double theta;
        if (dx == 0)
        {
            theta = 0;
        }
        else
        {
            theta = atan(dy/dx);
        }

        // expand
        r *= expansion;

        // rotate
        theta += rotation;

        nx = ox + r * cos(theta);
        ny = oy + r * sin(theta);

        // translate
        nx += translationX;
        ny += translationY;
    }
};


class ObjectHistory
{
private:
    std::vector<Transform> previous;
    ImgData mostRecent;
public:
    ObjectHistory();
};



class TrackingContext
{
    ImgData im1, im2;

    ImgData* current;
    ImgData* prev;


public:
    TrackingContext();
};

#endif // TRACKINGCONTEXT_H
