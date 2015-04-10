#include "out/reconstructor.h"

#include "util/logger.h"

Reconstructor::Reconstructor()
{
}

void Reconstructor::contribute(const Image& image,
                const Point3d& location,
                const Point3d& direction)
{
    auto pend = image.getPoints().end();
    for (auto pit = image.getPoints().begin(); pit != pend; ++pit)
    {
        int id = pit->first;
        auto it = histories.find(id);
        if (it == histories.end())
        {
            getLog() << "Tracking point " << pit->first << std::endl;
            histories.insert(std::pair<int, FeatureHistory>{id, FeatureHistory{id}});
            it = histories.find(id);
        }

        it->second.contribute(location, direction);
    }
}

void Reconstructor::listPoints(std::ostream& out) const
{
    auto end = histories.end();
    for (auto it = histories.begin(); it != end; ++it)
    {
        out << it->first << ": " << it->second.getEstimate() << std::endl;
    }
}
