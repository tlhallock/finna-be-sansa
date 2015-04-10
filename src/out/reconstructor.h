#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H

#include "out/featurehistory.h"
#include "out/image.h"


class Reconstructor
{
    std::map<int, FeatureHistory> histories;
public:
    Reconstructor();

    void contribute(const Image& image,
                    const Point3d& location,
                    const Point3d& direction);

    void listPoints(std::ostream& out) const;
};

#endif // RECONSTRUCTOR_H
