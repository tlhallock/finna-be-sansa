#ifndef SCENE_H
#define SCENE_H

#include "in/point3d.h"
#include <vector>


class Scene
{
private:
    std::vector<Point3d> pnts;

public:
    Scene(int numPnts, double exp)
    {
        for (int i=0;i<numPnts;i++)
        {
            pnts.push_back((Point3d{}*2.0-1.0) * exp);
        }
    }

    Scene() {}

    const std::vector<Point3d>& getPoints() const { return pnts; }

    void addFeature(const Point3d& p) { pnts.push_back(p); }

    friend std::ostream& operator<<(std::ostream& out, const Scene& s)
    {
        auto end = s.pnts.end();
        for (auto it = s.pnts.begin(); it != end; ++it)
        {
            out << *it << '\n';
        }
        return out << "=====================================" << std::endl;
    }
};

#endif // SCENE_H
