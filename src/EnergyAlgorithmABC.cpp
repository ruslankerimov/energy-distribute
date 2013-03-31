#include <iostream>
#include "EnergyAlgorithmABC.h"

using namespace std;

EnergyAlgorithmABC * algorithm1;

double fitness_wrap1(vector <double> cords)
{
	double ret = algorithm1->fitness(cords);

	return ret;
}

EnergyAlgorithmABC::EnergyAlgorithmABC(string input, string output):
        EnergyAlgorithm(input, output)
{
    vector <pair <double, double> > limits;
    for (int i = 0, size = genBus.size(); i < size; ++i)
    {
        EnergyBus * bus = genBus[i];
        if (bus->isBalancedBus())
        {
         continue;
        }
        double * powerLimits = bus->getPowerGenLimits();
        limits.push_back(make_pair(powerLimits[0], powerLimits[1]));
     }

    ABCconfig config;
    config.limits = limits;
    config.fitness = fitness_wrap1;
    config.max_iterations = 100;
    config.count_of_best_areas = 2;
    config.count_of_perspective_areas = 2;
    config.count_of_bees_to_best_area = 8;
    config.count_of_perspective_areas = 2;
    config.count_of_scout_bees = 10;
    config.neighbourhood_value = 0.3;

    algorithm1 = this;

    handle = new ABC(config);
}

void EnergyAlgorithmABC::solve()
{
    start_solve();

    fill(handle->solve());
    calculate();

    stop_solve();
}
