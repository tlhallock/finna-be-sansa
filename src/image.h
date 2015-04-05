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
public:
    double width, height;
private:
    std::map<int, ProjectedPoint> points;

public:
    Image(double width_, double height_) : width{width_}, height{height_} {}

    void clear() { points.clear(); }
    void add(const ProjectedPoint& p)
    {
        points.insert(std::pair<int, ProjectedPoint>{p.getId(), p});
    }

    const std::map<int, ProjectedPoint>& getPoints() const { return points; }

    friend std::ostream& operator<<(std::ostream& out, const Image& i)
    {
        auto end = i.points.end();
        for (auto it=i.points.begin(); it != end; ++it)
        {
            out << it->second << '\n';
        }
        return out << "-----------------------\n";
    }

    int visualize(int width, std::ostream& out) const;
};

#endif // IMAGE_H
