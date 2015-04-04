#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "point3d.h"

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

    const std::vector<Point3d>& getPoints() const { return pnts; }

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
