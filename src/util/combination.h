#ifndef COMBINATION_H
#define COMBINATION_H

#include <iostream>

class Combination
{
public:
    int n;
    int k;
    int *cvalue;
public:
    Combination(int n, int k);
    ~Combination();

    void reset(int loc=0);
    bool increment();
    friend std::ostream& operator<<(std::ostream& out, const Combination& c);
};

#endif // COMBINATION_H
