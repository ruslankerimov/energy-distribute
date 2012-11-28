#ifndef ENERGY_ALGORITHM_BEE_H_INCLUDED
#define ENERGY_ALGORITHM_BEE_H_INCLUDED

#include <vector>
#include <algorithm>
#include "EnergyAlgorithm.h"

using namespace std;

class EnergyAlgorithmBee:
        public EnergyAlgorithm
{
private:
    int count_of_scout_bees;
    int count_of_best_areas;
    int count_of_perspective_areas;
    int count_of_bees_to_best_area;
    int count_of_bees_to_perspective_area;
    int count_of_bees;
    int max_iterations;
    double neighbourhood_value;
public:
    EnergyAlgorithmBee(string, string);

    void solve();
};

#endif
