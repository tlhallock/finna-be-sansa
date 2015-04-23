#ifndef TRACKINGCONTEXT_H
#define TRACKINGCONTEXT_H

#include <vector>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define MAX_X_TRANS 100
#define MAX_Y_TRANS 100
#define MAX_X_EXP    2
#define MAX_Y_EXP    2
#define EXP_FACTOR   1.2


void writeBox(cv::Mat& myCopy,
          int px, int py,
          int w, int h,
          const cv::Scalar& c)
{
    cv::line(myCopy, cv::Point{px    , py    }, cv::Point{px + w, py    }, c, 2);
    cv::line(myCopy, cv::Point{px + w, py    }, cv::Point{px + w, py + h}, c, 2);
    cv::line(myCopy, cv::Point{px + w, py + h}, cv::Point{px    , py + h}, c, 2);
    cv::line(myCopy, cv::Point{px    , py + h}, cv::Point{px    , py    }, c, 2);
}



class ImgData
{
private:
public:
    double *red, *green, *blue;
    int width, height;
    int mwidth, mheight;

public:
    ImgData()
        : red{nullptr}
        , green{nullptr}
        , blue{nullptr}
        , width{-1}
        , height{-1}
        , mwidth{-1}
        , mheight{-1} {}

    ImgData(const cv::Mat& mat) : ImgData()
    {
        setData(mat);
    }

    ~ImgData()
    {
        delete[] red;
        delete[] green;
        delete[] blue;
    }

    ImgData(const ImgData& other)
        : ImgData{}
    {
        (*this) = other;
    }
    ImgData& operator=(const ImgData& other)
    {
        width = other.width;
        height = other.height;
        ensureSize();
        for (int i=0;i<width*height;i++)
        {
            red[i] = other.red[i];
            green[i] = other.green[i];
            blue[i] = other.blue[i];
        }
        return *this;
    }

    void setSubImg(const cv::Mat& larger, int x, int y, int w, int h)
    {
        setData(cv::Mat{larger, cv::Rect(x, y, w, h)});
    }
    
    void setData(const cv::Mat& mat)
    {
        width = mat.cols;
        height = mat.rows;
        ensureSize();

        for (int i=0;i<width;i++)
        {
            for (int j=0;j<height;j++)
            {
                const cv::Vec3b intensity = mat.at<cv::Vec3b>(j, i);

                blue [i * height + j] = intensity.val[0] / 255.0;
                green[i * height + j] = intensity.val[1] / 255.0;
                red  [i * height + j] = intensity.val[2] / 255.0;
            }
        }
    }

private:
    void ensureSize()
    {
        if (width > mwidth || height > mheight)
        {
            delete[] red;
            delete[] green;
            delete[] blue;
            red   = new double[width * height];
            green = new double[width * height];
            blue  = new double[width * height];

            mwidth = width;
            mheight = height;
        }
    }
};


class TrackedObject
{
public:
    // Right now just a rectangle
    int x, y, w, h;
    ImgData current;
public:
    TrackedObject(const cv::Mat& orig, int x_, int y_, int w_, int h_)
        : x{x_}
        , y{y_}
        , w{w_}
        , h{h_}
    {
        current.setSubImg(orig, x, y, w, h);
    }

    friend std::ostream& operator<<(std::ostream& out, const TrackedObject& o)
    {
        return out << "{x=" << o.x <<",y="<<o.y<<",w="<<o.w<<",h="<<o.h<<"}";
    }
};


#define ROT 0

class Transform
{
private:
public:
    int cx, cy;
#if ROT
    double rotation;
#endif
    double expansionX,   expansionY;
    int translationX, translationY;

public:
    Transform()
        : cx{0}
        , cy{0}
#if ROT
        , rotation{0}
#endif
        , expansionX{1}
        , expansionY{1}
        , translationX{0}
        , translationY{0} {}

    friend std::ostream& operator<<(std::ostream& out, const Transform& t)
    {
        out << "{";
        out << "cx=" << t.cx << ",cy=" << t.cy << ",";
        out << "ex=" << t.expansionX << ",ey=" << t.expansionY << ",";
        out << "tx=" << t.translationX << ",ty=" << t.translationY << "}";
        return out;
    }
    
    double currentCost(const ImgData& prev, const ImgData& next, const TrackedObject& obj)
    {
        cx = obj.x + obj.w  / 2;
        cy = obj.y + obj.h / 2;

        double cost = 0;
        for (int i=0;i<obj.w;i++)
        {
            for (int j=0;j<obj.h;j++)
            {
                int px, py;
                int ox, oy;

                ox = obj.x + i;
                oy = obj.y + j;

                map(ox, oy, px, py);

                if (px < 0 || py < 0
                        || px > next.width || py > next.height)
                {
                    return -1.0;
                }


                double dcr;
                double dcg;
                double dcb;

                dcr = prev.blue [ox * prev.height + oy] - next.blue [px * next.height + py]; dcr *= dcr; cost += dcr;
                dcg = prev.green[ox * prev.height + oy] - next.green[px * next.height + py]; dcg *= dcg; cost += dcg;
                dcb = prev.red  [ox * prev.height + oy] - next.red  [px * next.height + py]; dcb *= dcb; cost += dcb;
            }
        }
        return sqrt(cost);
    }

    void visualize(cv::Mat& prev, cv::Mat& next, const TrackedObject& obj, cv::Scalar& color)
    {
        writeBox(prev, obj.x, obj.y, obj.w, obj.h, color);
        int nx, ny, nw, nh;
        map(obj.x, obj.y, nx, ny);
        map(obj.x + obj.w, obj.y + obj.h, nw, nh);
        writeBox(next, nx, ny, nw - nx, nh - ny, color);
    }

    void map(const int ox, const int oy, int& nx, int& ny)
    {
        double dx = ox - cx;
        double dy = oy - cy;

#if ROT
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
#else
        dx *= expansionX;
        dy *= expansionY;

        nx = ox + dx;
        ny = oy + dy;
#endif

        // translate
        nx += translationX;
        ny += translationY;
    }
};



Transform seek(const ImgData& prev, const ImgData& next, const TrackedObject& obj, const Transform& guess,


               cv::Mat& debug1, cv::Mat& debug2)
{
    Transform min;
    Transform cur;
    double mCost = DBL_MAX;

    for (cur.translationX = guess.translationX-MAX_X_TRANS; cur.translationX < guess.translationX+MAX_X_TRANS; cur.translationX += 1)
    for (cur.translationY = guess.translationY-MAX_Y_TRANS; cur.translationY < guess.translationY+MAX_Y_TRANS; cur.translationY += 1)
    for (cur.expansionX = 0.5; cur.expansionX < MAX_X_EXP; cur.expansionX *= EXP_FACTOR)
    for (cur.expansionY = 0.5; cur.expansionY < MAX_Y_EXP; cur.expansionY *= EXP_FACTOR)
    {
        double cCost = cur.currentCost(prev, next, obj);
        if (cCost < 0.0)
        {
            continue;
        }

        std::cout << "The original object is " << obj << std::endl;
        std::cout << "The transform is " << cur << std::endl;

        std::cout << "Cost is: " << cCost << std::endl;
        cv::Mat im1 = debug1.clone();
        cv::Mat im2 = debug2.clone();


        cv::Scalar c(0, 0, 0);
        cur.visualize(im1, im2, obj, c);

        cv::imshow("Original window", im1);
        cv::imshow("Next window", im2);

        cv::waitKey();

        if (cCost > mCost)
        {
            continue;
        }
        mCost = cCost;
        min = cur;
    }

    return min;
}

#if 0
class ObjectHistory
{
private:
    std::vector<Transform> previous;
    ImgData mostRecent;
public:
    ObjectHistory();
};
#endif



class TrackingContext
{
    ImgData im1, im2;

    ImgData* current;
    ImgData* prev;

public:
    TrackingContext() {}
};

#endif // TRACKINGCONTEXT_H
