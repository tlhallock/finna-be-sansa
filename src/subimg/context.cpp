#include "subimg/context.h"
#include "subimg/subcontext.h"

#include <iostream>

Context::Context(const std::set<int>& subsizes_)
        : Asx{-1}
        , Asy{-1}
        , Asx2{-1}
        , Asad{nullptr}
        , Af{nullptr}
        , Ba{nullptr}
        , Bf{nullptr}
        , crosscorrs{nullptr}
        , subsizes{subsizes_}
        , currentFrameId{0}
        , currentFrame{} {}

Context::~Context()
{
    freeAllMemory();
}

void Context::freeAllMemory()
{
    clearSubSizes();
    free(Asad);
    free(Af);
    free(Ba);
    free(Bf);
    free(crosscorrs);
}

void Context::setOriginal(const cv::Mat& mat)
{
    int w = mat.cols - (mat.cols % 2);
    int h = mat.rows - (mat.rows % 2);
    if (Asx != w || Asy != h)
    {
        if (Asx != -1)
        {
            std::cerr << "Warning: Changing size of input Matrix" << std::endl;
            // We would have to free some stuff now...
        }
        freeAllMemory();

        Asx = w;
        Asy = h;
        Asx2 = Asx/2+1;
        Asad = (uint8_t*)malloc(sizeof(uint8_t)*Asx*Asy);

        Af = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Asx2*Asy);
        Ba = (double*)fftw_malloc(sizeof(double)*Asx*Asy);
        Bf = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Asx2*Asy);
        crosscorrs = (double*)fftw_malloc(sizeof(double)*Asx*Asy);

        forwardB = fftw_plan_dft_r2c_2d(Asy,Asx,Ba,Bf,         FFTW_ESTIMATE);
        backward = fftw_plan_dft_c2r_2d(Asy,Asx,Bf,crosscorrs, FFTW_ESTIMATE);

        clearSubSizes();
        auto end = subsizes.end();
        for (auto it = subsizes.begin(); it != end; ++it)
            subsizeClasses.insert(std::pair<int, SubSizeClass*> {*it, new SubSizeClass{this, *it}});
    }

    READ(mat, Asad);

    auto end = subsizeClasses.end();
    for (auto it = subsizeClasses.begin(); it != end; ++it)
        it->second->setOriginal();

    currentFrameId++;
    currentFrame = mat;
}

bool Context::getShift(roi& region, int& x, int& y)
{
    auto it = subsizeClasses.find(region.getSubImage().rows);
    if (it == subsizeClasses.end())
    {
        x = -1;
        y = -1;
        return false;
    }

    it->second->setSub(region.getSubImage());
    it->second->compute(x, y);

    return region.found(currentFrameId, currentFrame, x, y);
}

void Context::clearSubSizes()
{
    auto end = subsizeClasses.end();
    for (auto it = subsizeClasses.begin(); it != end; ++it)
    {
        delete it->second;
    }
    subsizeClasses.clear();
}


void Context::search(cv::Mat& haystack, std::vector<roi>& needles)
{
    setOriginal(haystack);

    int j = 0;
    auto end = needles.end();
    for (auto it = needles.begin(); it != end; ++it)
    {
        cv::imwrite("imgs/debug_" + std::to_string(currentFrameId) + "." + std::to_string(j) + ".png", currentFrame);
        cv::imwrite("imgs/debug_sub_" + std::to_string(currentFrameId) + "." + std::to_string(j) + ".png", it->getSubImage());

        std::cout << "In: " << it->getX() << " " << it->getY() << "---";

        int sadx;
        int sady;
        getShift(*it, sadx, sady);

        std::cout << "Out: " << sadx << ", " << sady << std::endl;
        it->saveBox(haystack, currentFrameId, "idk", cv::Scalar{0,0,255});

        j++;
    }

    currentFrame.release();
}
