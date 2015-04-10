#include "out/reconstructor.h"

Reconstructor::Reconstructor()
{
}

void Reconstructor::contribute(const Image& image,
                const Point3d& location,
                const Point3d& direction)
{
    auto pend = image.getPoints().end();
    for (auto pit = image.getPoints().end(); pit != pend; ++pit)
    {
        auto it = histories.find(pit->first);
        if (it == histories.end())
        {
            histories.insert(std::pair<int, FeatureHistory>{pit->first, FeatureHistory{pit->first}});
            it = histories.find(pit->first);
        }

        it->second.contribute(location, direction);
    }
}

void Reconstructor::listPoints(std::ostream& out)
{
    auto end = histories.end();
    for (auto it = histories.begin(); it != end; ++it)
    {
        out << it->first << ": " << it->second.getEstimate() << std::endl;
    }
}
