#ifndef FEATUREHISTORY_H
#define FEATUREHISTORY_H

#include "in/point3d.h"
#include "util/combination.h"

#define HISTORY_SIZE          5
#define NUM_TO_AVERAGE        2
#define LINE_SEGMENT_LENGTH   5
#define RANSAC_ITERATIONS     5

class Line
{
public:
    double p11, p12, p13;
    double p21, p22, p23;

    Line(){}
    Line(const Point3d& origin, const Point3d& direction);

    inline double sqDistanceTo(const double t1, const double t2, const double t3) const;
    inline void partialSqDistanceTo(
            const double t1,  const double t2,  const double t3,
            double& pt1, double &pt2, double &pt3) const;

    friend std::ostream& operator<<(std::ostream& out, const Line& l)
    {
       return out << "[" << l.p11 << ", " << l.p12 << ", " << l.p13 << ": "
                      << l.p21 << ", " << l.p22 << ", " << l.p23 << "]";
    }
};

class FeatureHistory
{
private:
    int id;
    int size;
    double currentCost;
    Point3d bestGuess;

    int nextReplace;
    Line lines[HISTORY_SIZE];
    bool used[HISTORY_SIZE];
    Combination c{HISTORY_SIZE, NUM_TO_AVERAGE};
public:
    FeatureHistory(int trackingId);

    void contribute(const Point3d& origin, const Point3d& direction);
    Point3d getEstimate() const { return bestGuess; }
};

#endif // FEATUREHISTORY_H
