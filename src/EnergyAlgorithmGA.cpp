#define INSTANTIATE_REAL_GENOME
#include <iostream>
#include "EnergyAlgorithmGA.h"

using namespace std;

EnergyAlgorithmGA * algorithm2;

float fitness_wrap2(GAGenome & g)
{
	double ret = algorithm2->fitness(algorithm2->genomeToVector(g));

	return ret;
}

EnergyAlgorithmGA::EnergyAlgorithmGA(string input, string output):
        EnergyAlgorithm(input, output)
{
    GARealAlleleSetArray alleles;
    for (int i = 0, size = genBus.size(); i < size; ++i)
    {
        EnergyBus * bus = genBus[i];
        if (bus->isBalancedBus())
        {
            continue;
        }
        double * powerLimits = bus->getPowerGenLimits();
        alleles.add(powerLimits[0], powerLimits[1], GAAllele::INCLUSIVE, GAAllele::INCLUSIVE);
    }

    GARealGenome genome(alleles, fitness_wrap2);
    genome.crossover(GARealGenome::OnePointCrossover);

    handle = new GASteadyStateGA(genome);

    GAParameterList params;
    GASteadyStateGA::registerDefaultParameters(params);
    params.set(gaNnGenerations, 100);
    params.set(gaNpopulationSize, 20);
    params.set(gaNpMutation, 0.4);
    params.set(gaNpCrossover, 0.6);
    handle->parameters(params);

    // @todo можно здесь подумать
    GANoScaling scaling;
    handle->scaling(scaling);

    GATournamentSelector selector;
    handle->selector(selector);

    algorithm2 = this;
}


vector <double> EnergyAlgorithmGA::genomeToVector(const GAGenome & g)
{
    vector <double> cords;
    GARealGenome& genome = (GARealGenome&) g;

    for (int i = 0, size = genWithoutBalanceBus.size(); i < size; ++i) // @todo
    {
        cords.push_back(genome.gene(i));
    }

    return cords;
}

void EnergyAlgorithmGA::solve()
{
    start_solve();

    handle->evolve();
    fill(genomeToVector(handle->statistics().bestIndividual()));
    calculate();

    stop_solve();
}
