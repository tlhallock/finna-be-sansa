#ifndef POINT2D_H
#define POINT2D_H

#include <iostream>
#include <iomanip>

class ProjectedPoint
{
private:
    int id;
public:
    double x, y;

public:
    ProjectedPoint(double x, double y);

    ProjectedPoint(double x_, double y_, int id_)
        : x{x_}
        , y{y_}
        , id{id_} {}

    friend std::ostream& operator<<(std::ostream& out, const ProjectedPoint& i)
    {
        return out << "{" << std::setw(10) << i.x << "," << std::setw(10) << i.y << "}";
    }

    ProjectedPoint& operator+=(const ProjectedPoint& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    void setId(int id_)
    {
        id = id_;
    }

    int getId() const
    {
        return id;
    }
};

#endif // POINT2D_H
