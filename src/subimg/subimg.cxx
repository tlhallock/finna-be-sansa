/**
 * A subimage finder (c) Werner Van Belle - 2007
 * mailto: werner@yellowcouch.org
 * License: GPL2
 */
#include <assert.h>
#include <math.h>
#include <fftw3.h>
#include <iostream>

#include "subimg/context.h"
#include "subimg/roi.h"

#define STEP_SIZE 50
#define NUM_IMAGES 2
#define START_IMAGE 20000
#define VIS 0
// "/media/rever/18D2394BD2392E7E/GOPR1596.MP4"
#define VIDEO_FILE "/media/thallock/8765-4321/DCIM/100GOPRO/GOPR1596.MP4"



int main(int argc, char **argv)
{
    srand(time(NULL));

    cv::VideoCapture cap {VIDEO_FILE};
    cv::Mat mat;


    cap.set(CV_CAP_PROP_POS_FRAMES, START_IMAGE);
    cap.read(mat);

    constexpr int NUM_SUB_IMAGES = 5;
    int width  = mat.cols / NUM_SUB_IMAGES;
    int height = mat.rows / NUM_SUB_IMAGES;

    width = std::min(width, height);
    width = width - width % 2;
    height = width;

    std::cout << "Sub image width and height: " << width << ", " << height << std::endl;

    std::vector<roi> needles;
    for (int i=0;i<NUM_SUB_IMAGES;i++)
        for (int j=0;j<NUM_SUB_IMAGES;j++)
            needles.push_back(roi{i * width, j * height, width, height, mat});


    std::set<int> subsizes;
    subsizes.insert(width);
    Context c{subsizes};

#if VIS
    cv::namedWindow("video", cv::WINDOW_AUTOSIZE);
#endif

    for (int i=1;i<NUM_IMAGES;i++)
    {
        std::cout << "Searching image " << i << std::endl;
        cap.set(CV_CAP_PROP_POS_FRAMES, START_IMAGE + i * STEP_SIZE);
        cap.read(mat);

        c.search(mat, needles);

#if VIS
        cv::imshow("video", mat);
        cv::waitKey(20);
#endif
    }


#if 0
    char *f1 = argv[1];
    char *f2 = argv[2];

    getEdges(f1);

        std::string outfile = split(f1, x, y, w, h);

        std::cout << "In: " << x << ", " << y << std::endl;
        red(original, x, y, w, h, "Original image");

        search(f2, outfile.c_str());
    }
#endif
    return 0;
}


#if 0


std::string split(const char *in, int x, int y, int w, int h)
{
#if 0
    if (argc < 2)
    {
        std::cerr << "Bad args." << std::endl;
        return "";
    }
#endif
    cv::Mat original = cv::imread(in, CV_LOAD_IMAGE_COLOR);

    cv::Mat roi(original, cv::Rect(x, y, w, h));

    std::string filename = in;
    auto ndx = filename.find_last_of("/\\");
    if (ndx != std::string::npos)
    {
        filename = filename.substr(ndx+1);
    }

    std::stringstream of;
    of << x << "_" << y << "_" << w << "_" << h << "_" << filename;

    std::cout << "Saving to " << of.str() << std::endl;

    cv::imwrite(of.str(), roi);

//    cv::imshow("roi", roi);
//    cv::waitKey();

    return of.str();
}


#if 0
        cv::Mat haystack = cv::imread(f1, CV_LOAD_IMAGE_COLOR);
        cv::Mat needle   = cv::imread(f2, CV_LOAD_IMAGE_COLOR);

        if (!haystack.data || !needle.data)
        {
            std::cout <<  "Could not open or find the image" << std::endl;
            return -1;
        }

        red(haystack, sadx, sady, Bsx, Bsy, "Haystack window");

        cv::namedWindow("needle window", cv::WINDOW_AUTOSIZE);
        cv::imshow("needle window", needle);

        cv::waitKey(0);
#endif



#endif
