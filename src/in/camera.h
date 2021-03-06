#ifndef CAMERA_H
#define CAMERA_H

#include "in/point3d.h"

class Scene;
class Image;

class Camera
{
private:
    Point3d focal;
    Point3d screen;
    double width;

    Point3d cameraXAxis;
    Point3d cameraYAxis;
public:
    Camera();

    void moveBy(double dx, double dy, double dz);
    void rotateBy(double dtheta, double dphi);

    void project(const Scene& scene, Image& output) const;
    double getWidth() const { return width; }

    Point3d location() const { return focal; }
    Point3d direction() const { return screen; }
};

#endif // CAMERA_H
