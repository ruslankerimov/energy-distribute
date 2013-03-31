#include <iostream>
#include "EnergyAlgorithmACO.h"

using namespace std;

EnergyAlgorithmACO * algorithm;

double fitness_wrap(vector <double> cords)
{
	double ret = algorithm->fitness(cords);

	return ret;
}

EnergyAlgorithmACO::EnergyAlgorithmACO(string input, string output):
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

    ACOconfig config;
    config.limits = limits;
    config.fitness = fitness_wrap;
    config.count_of_ants = 20;
    config.max_iterations = 250;
    config.count_of_good_ants = 5;
    config.count_of_bed_ants = 2;
    config.alpha = 0.95;
    config.ro = 0.8;
    config.delta_r = 0.1;
    config.propability_selecting = 0.9;
    config.tau_0 = 0.9;

    algorithm = this;

    handle = new ACO(config);
}

void EnergyAlgorithmACO::solve()
{
    start_solve();

    fill(handle->solve());
    calculate();

    stop_solve();
}
