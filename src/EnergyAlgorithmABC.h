#ifndef ENERGY_ALGORITHM_ABC_H_INCLUDED
#define ENERGY_ALGORITHM_ABC_H_INCLUDED

#include "ABC/src/ABC.h"
#include "EnergyAlgorithm.h"

using namespace std;

class EnergyAlgorithmABC:
        public EnergyAlgorithm
{
private:
    ABC * handle;
public:
    EnergyAlgorithmABC(string, string);

    void solve();
};

#endif
