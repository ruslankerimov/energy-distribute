#ifndef ENERGY_LINE_SET_INCLUDED
#define ENERGY_LINE_SET_INCLUDED

#include <vector>
#include <map>
#include "EnergyLine.h"

using namespace std;

class EnergyLineSet
{
private:
    map <int, EnergyLine *> lines;
    map <int, vector <EnergyLine*> > links;
    map <int, EnergyLine *>::iterator it;
public:
    EnergyLineSet();

    EnergyLineSet * addLine(EnergyLine *);

    EnergyLine * getLine(int, int);

    int size();

    double getActivePower();

    double getReactivePower();

    vector <EnergyLine*> getBusLines(int busNo);

    EnergyLine* operator [](int n);
};

#endif
