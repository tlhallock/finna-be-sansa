#ifndef MATRIX_H
#define MATRIX_H

#include "point3d.h"

typedef enum
{
    X, Y, Z
} axis;

class Matrix
{
    double elems[9];

public:
    Matrix()
    {
        elems[0] = 1; elems[1] = 0; elems[2] = 0;
        elems[3] = 0; elems[4] = 1; elems[5] = 0;
        elems[6] = 0; elems[7] = 0; elems[8] = 1;
    }

    Matrix(double x1, double x2, double x3,
           double x4, double x5, double x6,
           double x7, double x8, double x9)
    {
        elems[0] = x1; elems[1] = x2; elems[2] = x3;
        elems[3] = x4; elems[4] = x5; elems[5] = x6;
        elems[6] = x7; elems[7] = x8; elems[8] = x9;
    }


    Matrix(axis a, double theta)
    {
        switch(a)
        {
        case X:
            elems[0] =         (1); elems[1] =         (0); elems[2] =         (0);
            elems[3] =         (0); elems[4] =  cos(theta); elems[5] = -sin(theta);
            elems[6] =         (0); elems[7] =  sin(theta); elems[8] =  cos(theta);
            break;
        case Y:
            elems[0] =  cos(theta); elems[1] =         (0); elems[2] =  sin(theta);
            elems[3] =         (0); elems[4] =         (1); elems[5] =         (0);
            elems[6] = -sin(theta); elems[7] =         (0); elems[8] =  cos(theta);
            break;
        case Z:
            elems[0] =  cos(theta); elems[1] = -sin(theta); elems[2] =         (0);
            elems[3] =  sin(theta); elems[4] =  cos(theta); elems[5] =         (0);
            elems[6] =         (0); elems[7] =         (0); elems[8] =         (1);
            break;
        default:
            std::cout << "No such axis: " << a << std::endl;
            exit(-1);
        }
    }

    Point3d operator*(const Point3d& other)
    {
        return Point3d{
            elems[ 0] * other.x + elems[ 1]  * other.y + elems[ 2] * other.z,
            elems[ 3] * other.x + elems[ 4]  * other.y + elems[ 5] * other.z,
            elems[ 6] * other.x + elems[ 7]  * other.y + elems[ 8] * other.z,
        };
    }


    Matrix operator*(const Matrix& other)
    {
        Matrix returnValue;

        for (int i=0;i<3;i++)
        {
            for (int j=0;j<3;j++)
            {
                double sum = 0;
                for (int k=0;k<3;k++)
                {
                    sum += elems[i * 3 + k] * other.elems[k * 3 + j];
                }
                returnValue.elems[i * 3 + j] = sum;
            }
        }

        return returnValue;
    }

    Matrix operator*(double scalar)
    {

#if 0
        std::cout <<
        elems[0] * scalar << ", " <<
        elems[1] * scalar << ", " <<
        elems[2] * scalar << ", " <<
        elems[3] * scalar << ", " <<
        elems[4] * scalar << ", " <<
        elems[5] * scalar << ", " <<
        elems[6] * scalar << ", " <<
        elems[7] * scalar << ", " <<
        elems[8] * scalar << std::endl;
#endif

        return Matrix {
            elems[0] * scalar,
            elems[1] * scalar,
            elems[2] * scalar,
            elems[3] * scalar,
            elems[4] * scalar,
            elems[5] * scalar,
            elems[6] * scalar,
            elems[7] * scalar,
            elems[8] * scalar};
    }

    Point3d operator[](int index)
    {
        return Point3d{
            elems[0 * 3 + index],
            elems[1 * 3 + index],
            elems[2 * 3 + index]};
    }

#define WIDTH 10

    friend std::ostream& operator << (std::ostream& out, const Matrix& mat)
    {
          return out
                  << std::setw(WIDTH) << mat.elems[0] << std::setw(WIDTH) << mat.elems[1] << std::setw(WIDTH) << mat.elems[2] << '\n'
                  << std::setw(WIDTH) << mat.elems[3] << std::setw(WIDTH) << mat.elems[4] << std::setw(WIDTH) << mat.elems[5] << '\n'
                  << std::setw(WIDTH) << mat.elems[6] << std::setw(WIDTH) << mat.elems[7] << std::setw(WIDTH) << mat.elems[8] << '\n';
    }
};

#endif // MATRIX_H
