#ifndef CAMERA_H
#define CAMERA_H

#include "point3d.h"

class Scene;
class Image;

class Camera
{
private:
    Point3d focal;
    double dist;
    double width;

    double theta;
    double phi;
public:
    Camera();

    void moveBy(double dx, double dy, double dz);
    void rotateBy(double dtheta, double dphi);

    void project(const Scene& scene, Image& output) const;
    double getWidth() const { return width; }
};

#endif // CAMERA_H
