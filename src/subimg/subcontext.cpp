#include "subimg/subcontext.h"
#include "subimg/context.h"


int16_t* boxaverage(int16_t*input, int sx, int sy, int wx, int wy);
void window(int16_t * img, int sx, int sy, int wx, int wy);
void normalize(int16_t * img, int sx, int sy, int wx, int wy);



SubSizeClass::SubSizeClass (const Context *c_, int subsize)
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

void SubSizeClass::setOriginal()
{
    for (int i=0;i<c->Asx*c->Asy;i++)
        A[i] = c->Asad[i];
    normalize(A,c->Asx,c->Asy,wx,wy);
    for(int row = 0 ; row < c->Asy ; row++)
        for(int col = 0 ; col < c->Asx ; col++)
            Aa[col+c->Asx*row]=A[col+c->Asx*row];
    fftw_execute(forwardA);
}

void SubSizeClass::setSub(const cv::Mat& subimage)
{
    READ(subimage, B);
    for (int i=0;i<Bsx*Bsy;i++)
        Bsad[i] = B[i];
    normalize(B,Bsx,Bsy,wx,wy);

    for(int j = 0 ; j < c->Asx*c->Asy ; j++)
        c->Ba[j]=0;
    for(int row = 0 ; row < Bsy ; row++)
        for(int col = 0 ; col < Bsx ; col++)
            c->Ba[col+c->Asx*row]=B[col+Bsx*row];

    fftw_execute(c->forwardB);
    double norm=c->Asx*c->Asy;
    for(int j = 0 ; j < c->Asx2*c->Asy ; j++)
    {
        double a = c->Af[j][0];
        double b = c->Af[j][1];
        double c2 = c->Bf[j][0];
        double d = -c->Bf[j][1];
        double e = a*c2-b*d;
        double f = a*d+b*c2;
        c->Bf[j][0]=e/norm;
        c->Bf[j][1]=f/norm;
    }
    fftw_execute(c->backward);
}

void SubSizeClass::compute(int &sadx, int &sady)
{
    int sa=1+c->Asx/Bsx;
    int sb=1+c->Asy/Bsy;
    int64_t minsad=Bsx*Bsy*256L;
    for(int a = 0 ; a < sa ; a++)
    {
        int xl = a*Bsx;
        int xr = xl+Bsx;
        if (xr>c->Asx) continue;
        for(int b = 0 ; b < sb ; b++)
        {
            int yl = b*Bsy;
            int yr = yl+Bsy;
            if (yr>c->Asy) continue;

            // find the maximum correlation in this cell
            int cormxat=xl+yl*c->Asx;
            double cormx = c->crosscorrs[cormxat];
            for(int x = xl ; x < xr ; x++)
                for(int y = yl ; y < yr ; y++)
                {
                    int j = x+y*c->Asx;
                    if (c->crosscorrs[j]>cormx)
                        cormx=c->crosscorrs[cormxat=j];
                }
            int corx=cormxat%c->Asx;
            int cory=cormxat/c->Asx;

            // We dont want subimages that fall of the larger image
            if (corx+Bsx>c->Asx) continue;
            if (cory+Bsy>c->Asy) continue;

            int64_t sad=0;
            for(int x = 0 ; sad < minsad && x < Bsx ; x++)
            for(int y = 0 ; y < Bsy ; y++)
            {
                int j = (x+corx)+(y+cory)*c->Asx;
                int i = x+y*Bsx;
                sad+=llabs((int)Bsad[i]-(int)c->Asad[j]);
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
}

    SubSizeClass::~SubSizeClass()
    {
        free(A);
        free(Aa);
    }
