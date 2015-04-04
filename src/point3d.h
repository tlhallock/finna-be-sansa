#ifndef POINT3D_H
#define POINT3D_H

#include <iostream>
#include <iomanip>
#include <cmath>

class Point3d
{
private:
    int id;

public:
    double x, y, z;

public:
    Point3d();
    Point3d(double, double, double);

    friend std::ostream& operator<<(std::ostream& out, const Point3d& i)
    {
        return out << "{" << std::setw(10) << i.x << ","  << std::setw(10)<< i.y << "," << std::setw(10) << i.z << "}";
    }

    inline Point3d& operator+=(const Point3d& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;

        return *this;
    }

    inline int getId() const
    {
        return id;
    }

    inline friend Point3d operator-(const Point3d& l, const Point3d& r)
    {
        return Point3d{l.x - r.x, l.y - r.y, l.z - r.z};
    }

    inline friend Point3d operator+(const Point3d& l, const Point3d& r)
    {
        return Point3d{l.x + r.x, l.y + r.y, l.z + r.z};
    }

    inline double operator*(const Point3d& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Point3d operator*(double f) const
    {
        return Point3d{x * f, y * f, z * f};
    }

    inline Point3d operator-(double f) const
    {
        return Point3d{x - f, y - f, z - f};
    }

    inline double norm()
    {
        return sqrt(this->operator *(*this));
    }

    inline Point3d& operator/=(double scalar)
    {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            return *this;
    }
};

#endif // POINT3D_H
