#ifndef ENERGY_LINE_INCLUDED
#define ENERGY_LINE_INCLUDED

#include <iostream>
#include <vector>
#include <map>
#include <math.h>
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

    double getActivePower();

    double getReactivePower();

    EnergyBus * getFrom();

    EnergyBus * getTo();

    void display();
};

#endif
