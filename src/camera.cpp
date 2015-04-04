#include "camera.h"


#include "scene.h"
#include "image.h"
#include "matrix.h"

#define ABS(x) ((x)<0?(-x):(x))
#define DEBUG 0



#define EPS .0001

Camera::Camera()
    : focal {0, 0, 0}
    , dist{1.0}
    , width{1}
    , theta{0}
    , phi{0}
{

}



void Camera::moveBy(double dx, double dy, double dz)
{
    focal += Point3d{dx, dy, dz};
}

void Camera::rotateBy(double dtheta, double dphi)
{
    theta += dtheta;
    phi += dphi;
}





class Transform
{
    Point3d translation;
    double rot[9];
};



void Camera::project(const Scene& scene, Image& output) const
{

    output.clear();

    const Point3d center = focal + Point3d{
            focal.x + dist * cos(theta) * cos(phi),
            focal.y + dist * sin(theta) * cos(phi),
            focal.z + dist * sin(phi  )            };

    Matrix tmp = (Matrix(Y, phi) * (Matrix(Z, theta) * (Matrix{} * dist)));


    // Find transform that makes the plane at zero
    // Plane equation:
    // Ax + By + Cz = D
    Point3d normal      = tmp[0];
    Point3d cameraXAxis = tmp[1];
    Point3d cameraYAxis = tmp[2];

    double D = normal * center;

#if DEBUG
    std::cout << tmp << std::endl;

    std::cout << "theta: " << theta << std::endl;
    std::cout << "phi: " << phi << std::endl;
    std::cout << "dist: "        << dist << std::endl;
    std::cout << "Normal:      " << normal << std::endl;
    std::cout << "cameraXAxis: " << cameraXAxis << std::endl;
    std::cout << "cameraYAxis: " << cameraYAxis << std::endl;
    std::cout << "center:      " << center << std::endl;
    std::cout << "focal:       " << focal << std::endl;
    std::cout << "D = " << D << std::endl;
#endif


    // Ignore angles for this iteration

    auto end = scene.getPoints().end();
    for (auto it = scene.getPoints().begin(); it != end; ++it)
    {
        Point3d delta  = *it - focal;

        // Is it behind the camera?
        Point3d dirToPoint = *it - center;
        if (dirToPoint * normal <= 0)
        {
            continue;
        }


        delta /= delta.norm();

#if DEBUG
//        std::cout << "Projecting " << *it << std::endl;
//        std::cout << "delta " << delta << std::endl;
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
//        std::cout << "Num: " << num << ", den: " << den << std::endl;
#endif

        if (ABS(den) < EPS)
        {
#if DEBUG
            //std::cout << "Parallel" << std::endl;
#endif
            // parallel
            continue;
        }

        double t = num / den;

#if DEBUG
        //std::cout << "t = " << ABS(t) << std::endl;
#endif

        if (ABS(t) < EPS)
        {
#if DEBUG
            //std::cout << "on plane" << std::endl;
#endif
            // on plane
            continue;
        }

        Point3d diff = (focal + delta * t) - center;
        double x = diff * cameraXAxis;
        double y = diff * cameraYAxis;


#if DEBUG
        //std::cout << "Becomes " << x << ", " << y << std::endl;
#endif

        if (abs(x) >= width || abs(y) >= width)
        {
#if DEBUG
//            std::cout << "So tossed..." << std::endl;
#endif
            continue;
        }

#if DEBUG
        std::cout << x << ", " << y << std::endl;
#endif
        output.add(ProjectedPoint{x, y, it->getId()});
    }
}
