#ifndef ENERGY_LINE_INCLUDED
#define ENERGY_LINE_INCLUDED

#include <vector>
#include <map>
#include "EnergyBus.h"

using namespace std;

class EnergyLine
{
private:
    EnergyBus * from;
    EnergyBus * to;
    float R, X;
    double G, B;
public:
    EnergyLine(EnergyBus *, EnergyBus *, double, double);

    double getR();

    double getX();

    double getG();

    double getB();

    EnergyBus * getFrom();

    EnergyBus * getTo();
};

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

    vector <EnergyLine*> getBusLines(int busNo);

    EnergyLine* operator [](int n);
};

#endif
