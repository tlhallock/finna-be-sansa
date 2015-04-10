#include "combination.h"

#include <iomanip>

Combination::Combination(int n_, int k_)
    : n{n_}
    , k{k_}
    , cvalue {new int[k]}
{
    reset();
}


#if 1
Combination::Combination(const Combination& other)
    : n{other.n}
    , k{other.k}
    , cvalue {new int[k]}
{
    for (int i=0;i<k;i++)
    {
        cvalue[i] = other.cvalue[i];
    }
}
#endif

Combination::~Combination()
{
    delete[] cvalue;
}

void Combination::reset(int ndx)
{
    for (int i=ndx;i<k;i++)
    {
        cvalue[i] = (i==0?0:(cvalue[i-1]+1));
    }
}

bool Combination::increment()
{
    for (int i=k-1;i>=0;--i)
    {
        if (cvalue[i] >= n)
        {
            continue;
        }

        cvalue[i]++;
        reset(i+1);
        return true;
    }
    return false;
}

std::ostream& operator<<(std::ostream& out, const Combination& c)
{
        for (int i=0;i<c.k;i++)
        {
            out << std::setw(5) << c.cvalue[i] << ' ';
        }
        return out;
}

