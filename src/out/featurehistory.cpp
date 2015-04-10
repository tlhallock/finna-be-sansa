#include "out/featurehistory.h"
#include "util/logger.h"
#include <cfloat>
#include <cstdlib>
#include <set>



#define TOLERANCE .00000001
#define MAX_ITERS 10000

namespace
{

inline void increment(const Line& l,
           double t1, double t2, double t3,
           double &p1, double &p2, double &p3,
           double &cost)
{
        double sqdist = l.sqDistanceTo(t1, t2, t3);

        double dist = sqrt(sqdist);

        double pt1;
        double pt2;
        double pt3;

        l.partialSqDistanceTo(t1, t2, t3, pt1, pt2, pt3);

        p1 += 2 * dist * pt1;
        p2 += 2 * dist * pt2;
        p3 += 2 * dist * pt3;

        cost += sqdist;
}


inline double find_closest_point (Line *lines, const Combination& c, Point3d& retVal)
{
    double t1 = retVal.x;
    double t2 = retVal.y;
    double t3 = retVal.z;

    // Amount to move by
    double factor = DBL_MAX;

    // Partials
    double p1 = 0;
    double p2 = 0;
    double p3 = 0;

    // Current cost
    double cost = 0;

    for (int i=0; i<c.k; i++)
    {
        increment(lines[c.cvalue[i]],
            t1, t2, t3,
            p1, p2, p3,
            cost);
    }

    int count = 0;
    for (int i=0; i<MAX_ITERS; i++, count++)
    {
        if (cost < factor)
        {
            factor = cost;
        }
//        getLog() << "Point is " << t1 << ", " << t2 << ", " << t3 << std::endl;
//        getLog() << "Partials are " << p1 << ", " << p2 << ", " << p3 << std::endl;
//        getLog() << "Factor is " << factor << std::endl;
//        getLog() << "Distance is " << cost << std::endl;

        double n = sqrt(p1*p1 + p2*p2 + p3*p3);

        if (n < TOLERANCE)
        {
            break;
        }

        double nt1 = t1 - p1 / n * factor;
        double nt2 = t2 - p2 / n * factor;
        double nt3 = t3 - p3 / n * factor;

        double np1 = 0; double np2 = 0; double np3 = 0;
        double ncost = 0;

        for (int i=0; i<c.k; i++)
        {
            increment(lines[c.cvalue[i]],
                nt1, nt2, nt3,
                np1, np2, np3,
                ncost);
        }

        if (ncost < cost)
        {
            // Next point is closer.
            t1 = nt1; t2 = nt2; t3 = nt3;
            p1 = np1; p2 = np2; p3 = np3;
            cost = ncost;

            continue;
        }

        // Don't jump as far...
        factor /= 2;

//        if (cost - pcost < TOLERANCE)
        if (factor < TOLERANCE)
        {
            // We are only going to be jumping within the tolerance.
            break;
        }
    }

    getLog() << "iters = " << count << std::endl;
    retVal.x = t1;
    retVal.y = t2;
    retVal.z = t3;

    return cost;
}

inline double getCost(Line *lines, const Combination& c, Point3d& retVal)
{
    find_closest_point(lines, c, retVal);

    double cost = 0;
    for (int i=0;i<HISTORY_SIZE;i++)
    {
        cost += lines[i].sqDistanceTo(retVal.x, retVal.y, retVal.z);
    }
    return cost;
}

}


Line::Line(const Point3d &origin, const Point3d &direction)
    : p11{origin.x}
    , p12{origin.y}
    , p13{origin.z}
    , p21{origin.x + LINE_SEGMENT_LENGTH * direction.x}
    , p22{origin.y + LINE_SEGMENT_LENGTH * direction.y}
    , p23{origin.z + LINE_SEGMENT_LENGTH * direction.z} {}


double Line::sqDistanceTo(const double t1, const double t2, const double t3) const
{
    const double q1 = -(p12 - t2)*(p21 - t1) + (p11 - t1)*(p22 - t2);
    const double q2 = +(p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3);
    const double q3 = -(p13 - t3)*(p22 - t2) + (p12 - t2)*(p23 - t3);

    const double d1 = p11 - p21;
    const double d2 = p12 - p22;
    const double d3 = p13 - p23;

    return (q1*q1 + q2*q2 + q3*q3) / (d1*d1+d2*d2+d3*d3);
}


void Line::partialSqDistanceTo(
        const double t1,  const double t2,  const double t3,
        double& pt1, double &pt2, double &pt3) const
{
    const double d1 = p11 - p21;
    const double d2 = p12 - p22;
    const double d3 = p13 - p23;

    const double d = d1*d1+d2*d2+d3*d3;

    pt1 = -2*(((p12 - t2)*(p21 - t1) - (p11 - t1)*(p22 - t2))*(p12 - p22) + ((p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3))*(p13 - p23))/d;
    pt2 =  2*(((p12 - t2)*(p21 - t1) - (p11 - t1)*(p22 - t2))*(p11 - p21) - ((p13 - t3)*(p22 - t2) - (p12 - t2)*(p23 - t3))*(p13 - p23))/d;
    pt3 =  2*(((p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3))*(p11 - p21) + ((p13 - t3)*(p22 - t2) - (p12 - t2)*(p23 - t3))*(p12 - p22))/d;
}



void FeatureHistory::contribute(const Point3d& origin, const Point3d& direction)
{
    if (size < HISTORY_SIZE)
    {
        lines[size++] = Line{origin, direction};
        return;
    }

    lines[nextReplace] = Line{origin, direction};

    for (int i=0;i<RANSAC_ITERATIONS;i++)
    {
        Point3d tmpPoint3d;
        double cost = getCost(lines, c, tmpPoint3d);

        if (cost > currentCost)
        {
            continue;
        }

        bestGuess = tmpPoint3d;
        currentCost = cost;

        for (int j=0;j<HISTORY_SIZE;j++)
        {
            used[j] = false;
        }

        for (int i=0;i<c.k;i++)
        {
            used[c.cvalue[i]] = true;
        }

        if (!c.increment())
        {
            c.reset();
        }
    }

    do
    {
        nextReplace = rand() % HISTORY_SIZE;
    } while (!used[nextReplace]);
}




FeatureHistory::FeatureHistory(int trackingId)
    : id{trackingId}
    , size{0}
    , currentCost{DBL_MAX}
    , bestGuess{-1, -1, -1}
    , nextReplace{-1}
{
}
