#ifndef CONTEXT_H
#define CONTEXT_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fftw3.h>
#include <set>
#include "roi.h"


class SubSizeClass;


class Context
{
    friend class SubSizeClass;

private:
    int Asx, Asy, Asx2;

    uint8_t* Asad;
    fftw_complex * Af;
    double * Ba;
    fftw_complex * Bf;
    double * crosscorrs;

    fftw_plan forwardB;
    fftw_plan backward;

    std::set<int> subsizes;
    std::map<int, SubSizeClass*> subsizeClasses;

    int currentFrameId;
    cv::Mat currentFrame;

public:
    Context(const std::set<int>& subsizes);
    ~Context();

    void setOriginal(const cv::Mat& mat);
    bool getShift(roi& region, int& x, int& y);
    void search(const cv::Mat& haystack, std::vector<roi>& needles);

private:
    void clearSubSizes();
    void freeAllMemory();
};


#endif // CONTEXT_H
