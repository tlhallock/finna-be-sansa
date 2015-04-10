#include "in/camera.h"
#include "in/scene.h"
#include "out/image.h"
#include "util/logger.h"
#include "util/matrix.h"


//#define ABS(x) ((x)<0?(-x):(x))
#define DEBUG 0



#define EPS .0001

Camera::Camera()
    : focal {0, 0, 0}
    , screen {1, 0, 0}
    , width{1}
    , cameraXAxis{0, 1, 0}
    , cameraYAxis{0, 0, 1}
{

}



void Camera::moveBy(double dx, double dy, double dz)
{
    focal += Point3d{dx, dy, dz};
}

void Camera::rotateBy(double dtheta, double dphi)
{
    Matrix rotation = Matrix(Y, dphi) * Matrix(Z, dtheta);
    screen      = rotation * screen;
    cameraXAxis = rotation * cameraXAxis;
    cameraYAxis = rotation * cameraYAxis;
}





class Transform
{
    Point3d translation;
    double rot[9];
};



void Camera::project(const Scene& scene, Image& output) const
{
    output.clear();

    const Point3d center = focal + screen;
//    const Point3d center = focal + Point3d{
//            focal.x + dist * cos(theta) * cos(phi),
//            focal.y + dist * sin(theta) * cos(phi),
//            focal.z + dist * sin(phi  )            };


    // Find transform that makes the plane at zero
    // Plane equation:
    // Ax + By + Cz = D
    Point3d normal      = screen / screen.norm();

    double D = normal * center;

    // Ignore angles for this iteration

    auto end = scene.getPoints().end();
    for (auto it = scene.getPoints().begin(); it != end; ++it)
    {
        Point3d delta  = *it - focal;


#if DEBUG
    getLog() << "screen: " << screen << std::endl;
    getLog() << "dist: "        << screen.norm() << std::endl;
    getLog() << "Normal:      " << normal << std::endl;
    getLog() << "cameraXAxis: " << cameraXAxis << std::endl;
    getLog() << "cameraYAxis: " << cameraYAxis << std::endl;
    getLog() << "center:      " << center << std::endl;
    getLog() << "focal:       " << focal << std::endl;
    getLog() << "D = " << D << std::endl;
#endif


        // Is it behind the camera?
        if (*it * normal < D)
        {
#if DEBUG
            getLog() << *it << " is behind the camera" << std::endl;
#endif
            continue;
        }

//        Point3d dirToPoint = *it - center;
//        if (dirToPoint * delta <= 0)
//        {
//            continue;
//        }


        delta /= delta.norm();

#if DEBUG
        getLog() << "Projecting " << *it << std::endl;
        getLog() << "delta " << delta << std::endl;
#endif

        // want to a t so that
        // focal + t (dx, dy, dz)
        // satisfies Ax + By + Cz = D

        // A (focal.x + t*dx) + B (focal.y + t*dy) + C * (focal.z + t*dz) = D
        // A * focal.x + A*t*dx + B * focal.y + B*t*dy) + C*focal.z + C*t*dz = D
        // A*t*dx + B*t*dy + C*t*dz = D - A * focal.x - B * focal.y - C*focal.z
        // t * (A*dx + B*dy + C*dz) = D - A * focal.x - B * focal.y - C*focal.z
        // t = (D - A * focal.x - B * focal.y - C*focal.z) / (A*dx + B*dy + C*dz)
        double num = D - normal * focal;
        double den = normal * delta;

#if DEBUG
        getLog() << "Num: " << num << ", den: " << den << std::endl;
#endif

        if (ABS(den) < EPS)
        {
#if DEBUG
            getLog() << "Parallel" << std::endl;
#endif
            // parallel
            continue;
        }

        double t = num / den;

#if DEBUG
        getLog() << "t = " << ABS(t) << std::endl;
#endif

        if (ABS(t) < EPS)
        {
#if DEBUG
            getLog() << "on plane" << std::endl;
#endif
            // on plane
            continue;
        }

        Point3d diff = (focal + delta * t) - center;
        double x = diff * cameraXAxis;
        double y = diff * cameraYAxis;


#if DEBUG
        getLog() << "Becomes " << x << ", " << y << std::endl;
#endif

        if (abs(x) >= width || abs(y) >= width)
        {
#if DEBUG
            getLog() << "So tossed..." << std::endl;
#endif
            continue;
        }

#if DEBUG
        getLog() << x << ", " << y << std::endl;
#endif
        output.add(ProjectedPoint{x, y, it->getId()});
    }
}
