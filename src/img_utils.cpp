
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


#define GRAY(r, g, b) (((r)*11+(g)*16+(b)*5)/32)

void red(cv::Mat& m, int sadx, int sady, int Bsx, int Bsy, const std::string& name)
{
    for (int i=0;i<Bsy;i++)
    {
        for (int j=0;j<Bsx;j++)
        {
            m.at<cv::Vec3b>(sady + i, sadx + j)[0] = 0;
            m.at<cv::Vec3b>(sady + i, sadx + j)[1] = 0;
            m.at<cv::Vec3b>(sady + i, sadx + j)[2] = 255;
        }
    }

    cv::namedWindow(name,  cv::WINDOW_AUTOSIZE);
    cv::imshow(name, m);
}

void normalize(int16_t * img, int sx, int sy, int wx, int wy)
{
  /**
   * Calculate the mean background. We will subtract this
   * from img to make sure that it has a mean of zero
   * over a window size of wx x wy. Afterwards we calculate
   * the square of the difference, which will then be used
   * to normalize the local variance of img.
   */
  int16_t * mean = boxaverage(img,sx,sy,wx,wy);
  int16_t * sqr = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
  for(int j = 0 ; j < sx * sy ; j++)
    {
      img[j]-=mean[j];
      int16_t v = img[j];
      sqr[j]= v*v;
    }
  int16_t * var = boxaverage(sqr,sx,sy,wx,wy);
  /**
   * The normalization process. Currently still
   * calculated as doubles. Could probably be fixed
   * to integers too. The only problem is the sqrt
   */
  for(int j = 0 ; j < sx * sy ; j++)
    {
      double v = sqrt(fabs(var[j]));
      assert(std::isfinite(v) && v>=0);
      if (v<0.0001) v=0.0001;
      img[j]*=32/v;
      if (img[j]>127) img[j]=127;
      if (img[j]<-127) img[j]=-127;
    }
  /**
   * Mean was allocated in the boxaverage function
   * Sqr was allocated in thuis function
   * Var was allocated through boxaveragering
   */
  free(mean);
  free(sqr);
  free(var);

  /**
   * As a last step in the normalization we
   * window the sub image around the borders
   * to become 0
   */
  window(img,sx,sy,wx,wy);
}


int16_t* boxaverage(int16_t*input, int sx, int sy, int wx, int wy)
{
  int16_t *horizontalmean = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
  assert(horizontalmean);
  int wx2=wx/2;
  int wy2=wy/2;
  int16_t* from = input + (sy-1) * sx;
  int16_t* to = horizontalmean + (sy-1) * sx;
  int initcount = wx - wx2;
  if (sx<initcount) initcount=sx;
  int xli=-wx2;
  int xri=wx-wx2;
  for( ; from >= input ; from-=sx, to-=sx)
    {
      int64_t sum = 0;
      int count = initcount;
      for(int c = 0; c < count; c++)
    sum+=(int64_t)from[c];
      to[0]=sum/count;
      int xl=xli, x=1, xr=xri;
      /**
       * The area where the window is slightly outside the
       * left boundary. Beware: the right bnoundary could be
       * outside on the other side already
       */
      for(; x < sx; x++, xl++, xr++)
    {
      if (xl>=0) break;
      if (xr<sx)
        {
          sum+=(int64_t)from[xr];
          count++;
        }
      to[x]=sum/count;
    }
      /**
       * both bounds of the sliding window
       * are fully inside the images
       */
      for(; xr<sx; x++, xl++, xr++)
    {
      sum-=(int64_t)from[xl];
      sum+=(int64_t)from[xr];
      to[x]=sum/count;
    }
      /**
       * the right bound is falling of the page
       */
      for(; x < sx; x++, xl++)
    {
      sum-=(int64_t)from[xl];
      count--;
      to[x]=sum/count;
    }
    }

  /**
   * The same process as above but for the vertical dimension now
   */
  int ssy=(sy-1)*sx+1;
  from=horizontalmean+sx-1;
  int16_t *verticalmean = (int16_t*)malloc(sizeof(int16_t)*sx*sy);
  assert(verticalmean);

  for (int i=0;i<sx*sy;i++)
      verticalmean[i] = 0;

  to=verticalmean+sx-1;
  initcount= wy-wy2;
  if (sy<initcount) initcount=sy;
  int initstopat=initcount*sx;
  int yli=-wy2*sx;
  int yri=(wy-wy2)*sx;
  for(; from>=horizontalmean ; from--, to--)
    {
      int64_t sum = 0;
      int count = initcount;
      for(int d = 0 ; d < initstopat; d+=sx)
    sum+=(int64_t)from[d];
      to[0]=sum/count;
      int yl=yli, y = 1, yr=yri;
      for( ; y < ssy; y+=sx, yl+=sx, yr+=sx)
    {
      if (yl>=0) break;
      if (yr<ssy)
        {
          sum+=(int64_t)from[yr];
          count++;
        }
      to[y]=sum/count;
    }
      for( ; yr < ssy; y+=sx, yl+=sx, yr+=sx)
    {
      sum-=(int64_t)from[yl];
      sum+=(int64_t)from[yr];
      to[y]=sum/count;
    }
      for( ; y < ssy; y+=sx, yl+=sx)
    {
      sum-=(int64_t)from[yl];
      count--;
      to[y]=sum/count;
    }
    }
  free(horizontalmean);
  return verticalmean;
}

void window(int16_t * img, int sx, int sy, int wx, int wy)
{
  int wx2=wx/2;
  int sxsy=sx*sy;
  int sx1=sx-1;
  for(int x = 0 ; x < wx2 ; x ++)
    for(int y = 0 ; y < sxsy ; y+=sx)
      {
    img[x+y]=img[x+y]*x/wx2;
    img[sx1-x+y]=img[sx1-x+y]*x/wx2;
      }

  int wy2=wy/2;
  int syb=(sy-1)*sx;
  int syt=0;
  for(int y = 0 ; y < wy2 ; y++)
    {
     for(int x = 0 ; x < sx ; x ++)
       {
     /**
      * here we need to recalculate the stuff (*y/wy2)
      * to preserve the discrete nature of integers.
      */
     img[x + syt]=img[x + syt]*y/wy2;
     img[x + syb]=img[x + syb]*y/wy2;
       }
     /**
      * The next row for the top rows
      * The previous row for the bottom rows
      */
     syt+=sx;
     syb-=sx;
     }
}

int16_t* read_image(const cv::Mat& mat, int& sx, int&sy)
{
}
