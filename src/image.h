#ifndef IMAGE_H
#define IMAGE_H

#include <map>
#include <cfloat>

#include "point2d.h"



#define ABS(x)  ((x)<0?-(x):(x))
#define MAXV(x, y)  (ABS(x)<ABS(y)?ABS(y):ABS(x))
#define CW 4




#define PRINT_WIDTH 50

class Image
{
private:
    std::map<int, ProjectedPoint> points;

public:
    Image();

    void clear() { points.clear(); }
    void add(const ProjectedPoint& p)
    {
        points.insert(std::pair<int, ProjectedPoint>{p.getId(), p});
    }

    friend std::ostream& operator<<(std::ostream& out, const Image& i)
    {
        auto end = i.points.end();
        for (auto it=i.points.begin(); it != end; ++it)
        {
            out << it->second << '\n';
        }
        return out << "-----------------------\n";
    }

    int visualize(int width, std::ostream& out) const
    {
        int retVal = 0;

        int chars[PRINT_WIDTH][PRINT_WIDTH];

        for (int i=0;i<PRINT_WIDTH;i++)
        {
            for (int j=0;j<PRINT_WIDTH; j++)
            {
                chars[i][j] = -1;
            }
        }

        double max =  width;
        double min = -width;

        auto end = points.end();

#if 0
        double max = 0;
        double min = 0;
        for (auto it = points.begin(); it != end; ++it)
        {
            if (it->second.x > max)
            {
                max = it->second.x;
            }
            if (it->second.x < min)
            {
                min = it->second.x;
            }
            if (it->second.y > max)
            {
                max = it->second.y;
            }
            if (it->second.y < min)
            {
                min = it->second.y;
            }
        }
#endif

        for (auto it = points.begin(); it != end; ++it)
        {
            std::cout << width << std::endl;
            std::cout << it->second.x << ", " << it->second.y << std::endl;

            int x = (int) ((it->second.x - min) / (max-min) * PRINT_WIDTH);
            int y = (int) ((it->second.y - min) / (max-min) * PRINT_WIDTH);

            if (x < 0 || y < 0 || x >= PRINT_WIDTH || y >= PRINT_WIDTH)
            {
                std::cout << x << ", " << y << std::endl;
                exit (-1);
            }

            chars[x][y] = it->first;
        }

        for (int i=0;i<CW*PRINT_WIDTH+2;i++)
        {
            out << "-"; retVal++;
        }
        out << '\n'; retVal++;
        for (int i=0;i<PRINT_WIDTH;i++)
        {
            out << "|"; retVal++;
            for (int j=0;j<PRINT_WIDTH;j++)
            {
                if (chars[i][j] == -1)
                {
                    for (int i=0;i<CW;i++)
                    {
                        out << ' ';
                    }
                }
                else
                {
                    out << std::setw(CW) << chars[i][j];
                }
                retVal+=CW;
            }
            out << "|\n"; retVal++; retVal++;
        }
        for (int i=0;i<CW*PRINT_WIDTH+2;i++)
        {
            out << "-"; retVal++;
        }
        out << '\n'; retVal++;
    }
};

#endif // IMAGE_H
