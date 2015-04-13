#ifndef SUBCONTEXT_H
#define SUBCONTEXT_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fftw3.h>
#include <cctype>

#define GRAY(r, g, b) (((r)*11+(g)*16+(b)*5)/32)

#define READ(mat, ptr) \
assert(mat.cols>4 && mat.rows>4); \
do { \
    for(int x = 0 ; x < mat.cols ; x++) \
    { \
        for(int y = 0 ; y < mat.rows; y++) \
        { \
            const auto rgb = mat.at<cv::Vec3b>(y, x); \
            int b = rgb[0]; \
            int g = rgb[1]; \
            int r = rgb[2]; \
            ptr[x+y*mat.cols]=GRAY(r, g, b); \
        } \
    } \
} while (0)



class Context;

class SubSizeClass
{
private:
    friend class Context;
    const Context *c;

    int Bsx, Bsy;
    int wx, wy;
    int16_t* A;
    double * Aa;
    fftw_plan forwardA;
    int16_t* B;
    uint8_t* Bsad;

public:
    SubSizeClass (const Context *c_, int subsize);
    ~SubSizeClass();

    void setOriginal();
    void setSub(const cv::Mat& subimage);
    void compute(int &sadx, int &sady);
};


#endif // SUBCONTEXT_H
