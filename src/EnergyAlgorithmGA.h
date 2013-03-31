#ifndef ENERGY_ALGORITHM_GA_H_INCLUDED
#define ENERGY_ALGORITHM_GA_H_INCLUDED

#include "GA/ga/ga.h"
#include "GA/ga/GARealGenome.h"
#include "EnergyAlgorithm.h"

using namespace std;

class EnergyAlgorithmGA:
        public EnergyAlgorithm
{
private:
  GASteadyStateGA* handle;
public:
    EnergyAlgorithmGA(string, string);
    vector <double> genomeToVector(const GAGenome &);

    void solve();
};

#endif
