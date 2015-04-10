#include "out/featurehistory.h"
#include "util/logger.h"
#include <cfloat>
#include <cstdlib>
#include <set>



#define TOLERANCE .00000001
#define MAX_ITERS 100000

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


inline double find_closest_point (Line *lines, const Combination c, Point3d& retVal)
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


#if 0


class Line
{
private:
    Point3d p1;
    Point3d p2;

public:
    Line(Point3d p1, Point3d p2)
        : origin{o}
        , direction{d}
    {
        direction /= direction.norm();
    }

    double distanceTo(const Point3d& p)
    {
        // minimize: dist(origin + t * direction, p) =

        //minimize: (p_i - oritin_i + t * direction_i)^2
        //=> 2*(p_i - origin_i + t * direction_i) * direction_i = 0
        //=> 2*p_i* direction_i - 2*origin_i* direction_i + 2*t * direction_i^2 = 0
        //=> p_i* direction_i - origin_i* direction_i + t * direction_i^2 = 0
        //=> t * direction_i^2 = (origin_i - p_i) * direction_i
        // t = (sum_i (origin_i - p_i) * direction_i) / (sum_i direction_i^2)
//        double t = (origin - p) * direction / direction.norm();
        double t = (origin - p) * direction;
        Point3d closest = origin + direction * t;
        return closest.dist(p);
    }
};


p11, p12, p13, p21, p22, p23, t1, t2, t3 = var('p11, p12, p13, p21, p22, p23, t1, t2, t3')

p1 = vector((p11, p12, p13))
p2 = vector((p21, p22, p23))
t = vector((t1, t2, t3))




norm((t - p1).cross_product(t-p2)) / norm(p1 - p2)

((-(p12 - t2)*(p21 - t1) + (p11 - t1)*(p22 - t2))^2 + ((p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3))^2 + (-(p13 - t3)*(p22 - t2) + (p12 - t2)*(p23 - t3))^2) / ((p11 - p21)^2 + (p12 - p22)^2 + (p13 - p23)^2)



        void double sq_dist(
            const double p11, const double p12, const double p13,
            const double p21, const double p22, const double p23,
            const double t1,  const double t2,  const double t3)
        {
        }

        void double pt_sq_dist(
            const double p11, const double p12, const double p13,
            const double p21, const double p22, const double p23,
            const double t1,  const double t2,  const double t3,
            double& pt1, double &pt2, double &pt3)
        {
            pt1 = -2*(((p12 - t2)*(p21 - t1) - (p11 - t1)*(p22 - t2))*(p12 - p22) + ((p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3))*(p13 - p23))/((p11 - p21)^2 + (p12 - p22)^2 + (p13 - p23)^2);


            pt2 = 2*(((p12 - t2)*(p21 - t1) - (p11 - t1)*(p22 - t2))*(p11 - p21) - ((p13 - t3)*(p22 - t2) - (p12 - t2)*(p23 - t3))*(p13 - p23))/((p11 - p21)^2 + (p12 - p22)^2 + (p13 - p23)^2);

            pt3 = 2*(((p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3))*(p11 - p21) + ((p13 - t3)*(p22 - t2) - (p12 - t2)*(p23 - t3))*(p12 - p22))/((p11 - p21)^2 + (p12 - p22)^2 + (p13 - p23)^2);


            const double q1 = -(p12 - t2)*(p21 - t1) + (p11 - t1)*(p22 - t2);
            const double q2 = +(p13 - t3)*(p21 - t1) - (p11 - t1)*(p23 - t3);
            const double q3 = -(p13 - t3)*(p22 - t2) + (p12 - t2)*(p23 - t3);

            const double d1 = p11 - p21;
            const double d2 = p12 - p22;
            const double d3 = p13 - p23;

            return (q1*q1 + q2*q2 + q3*q3) / (d1*d1+d2*d2+d3*d3);
        }

solve(f.diff(t1), f.diff(t2), f.diff(t3) == 0, t1, t2 ,t3)

        double findCost(Line*lines, int num, const Point3d& bestGuess)
        {
            double cost = 0;
            for (int i=0;i<num;i++)
            {
                cost += lines[i].distanceTo(bestGuess);
            }
            return cost;
        }

        void findClosest(Line* lines, int num, Point3d& bestGuess)
        {
            double currentCost = findCost(lines, num, bestGuess);

            double t = 1.0;
            while (t > .0001)
            {
                bool improvement = true;
                while (improvement)
                {
                    improvement = false;

                    Point3d direc;
        #define TEST_FOR_IMPROVEMENT()                                  \
                    {                                                   \
                        Point3d alt = bestGuess + direc;                \
                        double newCost = findCost(lines, num, alt);     \
                        if (newCost<currentCost)                        \
                        {                                               \
                            currentCost = newCost;                      \
                            bestGuess = alt;                            \
                            improvement = true;                         \
                        }                                               \
                    }

                    direc = Point3d{ t,  0,  0}; TEST_FOR_IMPROVEMENT();
                    direc = Point3d{-t,  0,  0}; TEST_FOR_IMPROVEMENT();
                    direc = Point3d{ 0,  t,  0}; TEST_FOR_IMPROVEMENT();
                    direc = Point3d{ 0, -t,  0}; TEST_FOR_IMPROVEMENT();
                    direc = Point3d{ 0,  0,  t}; TEST_FOR_IMPROVEMENT();
                    direc = Point3d{ 0,  0, -t}; TEST_FOR_IMPROVEMENT();
                }
                t /= 2;
            }
        }




        int main(int argc, char **argv)
        {
            Line l[3];
            l[0].p11 = 1;
            l[0].p12 = 0;
            l[0].p13 = 0;

            l[0].p21 = 0;
            l[0].p22 = 0;
            l[0].p23 = 0;


            l[1].p11 = 0;
            l[1].p12 = 1;
            l[1].p13 = 0;

            l[1].p21 = 0;
            l[1].p22 = 0;
            l[1].p23 = 0;

            l[2].p11 = 0;
            l[2].p12 = 0;
            l[2].p13 = 1;

            l[2].p21 = 0;
            l[2].p22 = 0;
            l[2].p23 = 0;

            double t1 = 1;
            double t2 = 500;
            double t3 = -500;

            int ndx[3];
            ndx[0] = 0; ndx[1] = 1; ndx[2] = 2;

            find_closest_point (&l[0], &ndx[0], 3, t1, t2, t3);

            return 0;
        }

#endif
