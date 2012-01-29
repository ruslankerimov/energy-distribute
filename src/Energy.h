#ifndef ENERGY_INCLUDED
#define ENERGY_INCLUDED

#include <vector>
#include <ga/ga.h>
#include <math.h>

//#include "tinyxml/tinyxml.h"
#include "../lib/tinyxml/tinyxml.cpp"
#include "../lib/tinyxml/tinystr.cpp"
#include "../lib/tinyxml/tinyxmlerror.cpp"
#include "../lib/tinyxml/tinyxmlparser.cpp"

#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>

#include "EnergyBus.h"
#include "EnergyLine.h"
#include "Newton.h"

using namespace std;

class Energy
{
private:
    static Energy * instance;

    void parseData();
    void parseBusData();
    void parseLinesData();
    void parseGAparams();
    void calculate(GARealGenome, bool);
    void fillFromGenome(GARealGenome);
    double getLosses();

    string inputDir, outputDir;
    EnergyBus * balanceBus;
    EnergyLineSet lines;
    EnergyBusSet allBus;
    EnergyBusSet genBus;
    EnergyBusSet notGenBus;
    EnergyBusSet genWithoutBalanceBus;
    GAParameterList params;
public:
    Energy();
    static Energy * getInstance();
    float objective(GAGenome &);
    double getYCell(int, int, vector <double>);
    double getFCell(int, vector <double>);
    void print();
    void report();
    void solve();
};

float wrap_objective(GAGenome &);
double wrap_getYCell(int, int, vector <double>);
double wrap_getFCell(int, vector <double>);

#endif
