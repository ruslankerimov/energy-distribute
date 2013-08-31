#ifndef ENERGY_ALGORITHM_H_INCLUDED
#define ENERGY_ALGORITHM_H_INCLUDED

#include <vector>
#include <math.h>
#include <ctime>
#include <string>

#include "tinyxml/tinyxml.h"

#include "EnergyBus.h"
#include "EnergyBusSet.h"
#include "EnergyLine.h"
#include "EnergyLineSet.h"
#include "newton/Newton.h"

using namespace std;

class EnergyAlgorithm:
        public Newton
{
private:
    double getYCell(int, int, vector <double>);
    double getFCell(int, vector <double>);

    void parseBusData();
    void parseLinesData();

    string inputDir, outputDir;
protected:
    EnergyAlgorithm(string, string);

    EnergyBus * balanceBus;
    EnergyLineSet lines;
    EnergyBusSet allBus;
    EnergyBusSet genBus;
    EnergyBusSet notGenBus;
    EnergyBusSet genWithoutBalanceBus;

    clock_t t1, t2;

    void progress(int, int);
    void start_solve();
    void stop_solve();

    void display();
    void report();
    void calculate();
    void fill(vector <double>);
public:
    double fitness(vector <double>);
    void solve();
};

#endif
