/**
 * A subimage finder (c) Werner Van Belle - 2007
 * mailto: werner@yellowcouch.org
 * License: GPL2
 */
#include <assert.h>
#include <qcolor.h>
#include <math.h>
#include <fftw3.h>
#include <qimage.h>
#include <qrgb.h>
#include <qapplication.h>
#include <iostream>
#include <cctype>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#define READ(mat, ptr) \
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




#define MIN_SIZE 20

class Context;


class SubSizeClass
{
    const Context *c;

    int Bsx, Bsy;
    int16_t* A;
    double * Aa;
    int wx, wy;
    fftw_plan forwardA;
    int16_t* B;
    uint8_t* Bsad;

    SubSizeClass (const Context *c_, int subsize)
        : c{c_}
        , Bsx{subsize}
        , Bsy{subsize}
        , wx { Bsx / 2 }
        , wy { Bsy / 2 }
        , A  { (int16_t*)malloc(sizeof(int16_t)*c->Asx*c->Asy) }
        , Aa { (double*)fftw_malloc(sizeof(double)*c->Asx*c->Asy) }
        , forwardA { fftw_plan_dft_r2c_2d(c->Asy,c->Asx,Aa,c->Af, FFTW_ESTIMATE)}
        , B{(int16_t*)malloc(sizeof(int16_t)*Bsx*Bsy)}
        , Bsad{(uint8_t*)malloc(sizeof(uint8_t)*Bsx*Bsy)} {}

    void setOriginal()
    {
        for (int i=0;i<c->Asx*c->Asy;i++)
            A[i] = c->Asad[i];
        normalize(A,c->Asx,c->Asy,wx,wy);
        for(int row = 0 ; row < c->Asy ; row++)
          for(int col = 0 ; col < c->Asx ; col++)
            Aa[col+c->Asx*row]=A[col+c->Asx*row];
        fftw_execute(forwardA);
    }

    void setSub(const cv::Mat& subimage)
    {
        READ(subimage, B);
        for (int i=0;i<Bsx*Bsy;i++)
            Bsad[i] = B[i];
        normalize(B,Bsx,Bsy,wx,wy);

        for(int j = 0 ; j < c->Asx*c->Asy ; j++)
            c->Ba[j]=0;
        for(int row = 0 ; row < Bsy ; row++)
            for(int col = 0 ; col < Bsx ; col++)
                c->Ba[col+Asx*row]=B[col+Bsx*row];

        fftw_execute(c->forwardB);
        double norm=c->Asx*c->Asy;
        for(int j = 0 ; j < c->Asx2*c->Asy ; j++)
        {
            double a = c->Af[j][0];
            double b = c->Af[j][1];
            double c = c->Bf[j][0];
            double d = -c->Bf[j][1];
            double e = a*c-b*d;
            double f = a*d+b*c;
            c->Bf[j][0]=e/norm;
            c->Bf[j][1]=f/norm;
        }
        fftw_execute(c->backward);
    }

    void compute(int &sadx, int &sady, double *crosscors)
    {
        int sa=1+Asx/Bsx;
        int sb=1+Asy/Bsy;
        int64_t minsad=Bsx*Bsy*256L;
        for(int a = 0 ; a < sa ; a++)
          {
            int xl = a*Bsx;
            int xr = xl+Bsx;
            if (xr>Asx) continue;
            for(int b = 0 ; b < sb ; b++)
          {
            int yl = b*Bsy;
            int yr = yl+Bsy;
            if (yr>Asy) continue;

            // find the maximum correlation in this cell
            int cormxat=xl+yl*Asx;
            double cormx = crosscorrs[cormxat];
            for(int x = xl ; x < xr ; x++)
              for(int y = yl ; y < yr ; y++)
                {
              int j = x+y*Asx;
              if (crosscorrs[j]>cormx)
                cormx=crosscorrs[cormxat=j];
                }
            int corx=cormxat%Asx;
            int cory=cormxat/Asx;

            // We dont want subimages that fall of the larger image
            if (corx+Bsx>Asx) continue;
            if (cory+Bsy>Asy) continue;

            int64_t sad=0;
            for(int x = 0 ; sad < minsad && x < Bsx ; x++)
              for(int y = 0 ; y < Bsy ; y++)
                {
              int j = (x+corx)+(y+cory)*Asx;
              int i = x+y*Bsx;
              sad+=llabs((int)Bsad[i]-(int)Asad[j]);
                }

            if (sad<minsad)
              {
                minsad=sad;
                sadx=corx;
                sady=cory;
                // printf("* ");
              }
            // printf("Grid (%d,%d) (%d,%d) Sip=%g Sad=%lld\n",a,b,corx,cory,cormx,sad);
          }
    }

    ~SubSizeClass()
    {
        free(A);
        free(Aa);
    }
};

class Context
{
public:
    int Asx, Asy, Asx2;

    uint8_t* Asad;
    fftw_complex * Af;
    double * Ba;
    fftw_complex * Bf;
    double * crosscorrs;

    fftw_plan forwardB;
    fftw_plan backward;

    std::map<int, SubSizeClass*> subsizeClasses;

    Context()
        : Asx{-1}
        , Asy{-1}
        , Asx2{-1}
        , A{nullptr}
        , Asad{nullptr}
        , Af{nullptr}
        , Ba{nullptr}
        , Bf{nullptr}
        , crosscorrs{nullptr} {}

    ~Context()
    {
        clearSubSizes();
        free(Asad);
        free(Af);
        free(Ba)
        free(Bf);
        free(crosscorrs);
    }


    void setOriginal(const cv::Mat& mat, std::set<int>& subsizes)
    {
        int w = mat.cols - (mat.cols % 2);
        int h = mat.rows - (mat.rows % 2);
        if (Asx != w || Asy != h)
        {
            Asx = w;
            Asy = h;
            Asx2 = Asx/2+1;
            assert(sx>4 && sy>4);
            Asad = (uint8_t*)malloc(sizeof(uint8_t)*Asx*Asy);

            Aa = (double*)fftw_malloc(sizeof(double)*Asx*Asy);
            Af = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Asx2*Asy);
            Ba = (double*)fftw_malloc(sizeof(double)*Asx*Asy);
            Bf = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)*Asx2*Asy);
            crosscorrs = (double*)fftw_malloc(sizeof(double)*Asx*Asy);

            forwardB = fftw_plan_dft_r2c_2d(Asy,Asx,Ba,Bf,        /*FFTW_FORWARD |*/ FFTW_ESTIMATE);
            backward = fftw_plan_dft_c2r_2d(Asy,Asx,Bf,crosscorrs,/*FFTW_FORWARD |*/ FFTW_ESTIMATE);

            clearSubSizes();
            auto end = subsizes.end();
            for (auto it = subsizes.begin(); it != end; ++it)
                subsizeClasses.insert(std::pair<int, SubSizeClass*> {it->first, new SubSizeClass{Asx, Asy, Asad, it->first}});
        }

        READ(mat, Asad);

        auto end = subsizeClasses.end();
        for (auto it = subsizeClasses.begin(); it != end; ++it)
            it->second->setOriginal(Asx, Asy, Asad);
    }



    void clearSubSizes()
    {
        auto end = subsizeClasses.end();
        for (auto it = subsizeClasses.begin(); it != end; ++it)
        {
            delete it->second;
        }
        subsizeClasses.clear();
    }
};



int getEdges(const char *filename);
int16_t* boxaverage(int16_t*input, int sx, int sy, int wx, int wy);
void window(int16_t * img, int sx, int sy, int wx, int wy);
int16_t* read_image(const char* filename, int& sx, int&sy);



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


int search(const cv::Mat& haystack, std::vector<const cv::Mat>& needles)
{
    Context c;
    c.setOriginal(haystack, 100, 100);

    auto end = needles.end();
    for (auto it = needles.begin(); it != end; ++it)
    {
        c.setsub(needles);


        sub = c.getsub(needles);


        sub->setSub(it->second);
        int sadx;
        int sady;

        sub->compute(sadx, sady);

        std::cout << "Out: " << sadx << ", " << sady << std::endl;

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
    }




  return 0;
}



int main(int argc, char **argv)
{
    QApplication app(argc,argv);

    srand(time(NULL));

    char *f1 = argv[1];
    char *f2 = argv[2];

    getEdges(f1);

    for (int i=0;i<10000;i++)
    {
        cv::Mat original = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);
        int x = argc > 3 ? atoi(argv[3]) : (rand() % (original.cols - MIN_SIZE));
        int y = argc > 4 ? atoi(argv[4]) : (rand() % (original.rows - MIN_SIZE));
        int w = argc > 5 ? atoi(argv[5]) : std::min(MIN_SIZE + rand() % (original.cols - x - MIN_SIZE), 100);
        int h = argc > 6 ? atoi(argv[6]) : std::min(MIN_SIZE + rand() % (original.rows - y - MIN_SIZE), 100);

        std::string outfile = split(f1, x, y, w, h);

        std::cout << "In: " << x << ", " << y << std::endl;
        red(original, x, y, w, h, "Original image");

        search(f2, outfile.c_str());
    }

    return 0;
}
