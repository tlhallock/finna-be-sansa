#include "point3d.h"

#include <stdlib.h>


namespace
{
    static int nextId = 0;
}

Point3d::Point3d() : Point3d {rand() / (double) RAND_MAX, rand() / (double) RAND_MAX, rand() / (double) RAND_MAX} {}


Point3d::Point3d(double x_, double y_, double z_) : x{x_}, y{y_}, z{z_}, id{nextId++} {}
