#ifndef ENERGY_ALGORITHM_ACO_H_INCLUDED
#define ENERGY_ALGORITHM_ACO_H_INCLUDED

#include "ACO/src/ACO.h"
#include "EnergyAlgorithm.h"

using namespace std;

class EnergyAlgorithmACO:
        public EnergyAlgorithm
{
private:
    ACO * handle;
public:
    EnergyAlgorithmACO(string, string);

    void solve();
};

#endif
