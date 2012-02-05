#ifndef ENERGY_INCLUDED
#define ENERGY_INCLUDED

#include <vector>
#include <math.h>
#include <ctime>
#include <string>

#include "ga/ga.h"
#include "ga/GARealGenome.h"
#include "tinyxml/tinyxml.h"

#include "EnergyBus.h"
#include "EnergyBusSet.h"
#include "EnergyLine.h"
#include "EnergyLineSet.h"
#include "Newton.h"

using namespace std;

class Energy
{
private:
    Energy();
    Energy(Energy &);
    static Energy * instance;

    float objective(GAGenome &);
    double getYCell(int, int, vector <double>);
    double getFCell(int, vector <double>);

    void clearData();
    void parseData();
    void parseBusData();
    void parseLinesData();
    void parseGAparams();
    void calculate();
    void fillFromGenome(GARealGenome);

    string inputDir, outputDir;
    EnergyBus * balanceBus;
    EnergyLineSet lines;
    EnergyBusSet allBus;
    EnergyBusSet genBus;
    EnergyBusSet notGenBus;
    EnergyBusSet genWithoutBalanceBus;
    GAParameterList params;

    clock_t t1, t2;
public:
    static Energy * getInstance();

    static float wrap_objective(GAGenome &);
    static double wrap_getYCell(int, int, vector <double>);
    static double wrap_getFCell(int, vector <double>);

    void setup(string, string);
    void display();
    void progress(float);
    void report();
    void solve();
};

#endif
