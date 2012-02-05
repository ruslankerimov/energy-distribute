#ifndef ENERGY_BUS_SET_INCLUDED
#define ENERGY_BUS_SET_INCLUDED

#include <map>
#include "EnergyBus.h"

using namespace std;

class EnergyBusSet
{
private:
    map <int, EnergyBus *> buses;
    map <int, EnergyBus *>::iterator it;
public:
    EnergyBusSet();

    EnergyBus* getBus(int);

    EnergyBusSet* addBus(EnergyBus *);

    double cost();

    double getActivePowerLoad();

    double getReactivePowerLoad();

    double getActivePowerGen();

    double getReactivePowerGen();

    int size();

    bool checkRestractions();

    EnergyBus* operator [](int);
};

#endif
