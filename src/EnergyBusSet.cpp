#include "EnergyBusSet.h"

EnergyBusSet::EnergyBusSet()
{

}

EnergyBus* EnergyBusSet::getBus(int n)
{
    return buses[n];
}

EnergyBusSet* EnergyBusSet::addBus(EnergyBus * bus)
{
    buses[bus->getNo()] = bus;
    return this;
}

double EnergyBusSet::cost()
{
    double ret = 0;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        ret += it->second->cost();
    }

    return ret;
}

double EnergyBusSet::getActivePowerLoad()
{
    double ret = 0;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        ret += it->second->getActivePowerLoad();
    }

    return ret;
}

double EnergyBusSet::getReactivePowerLoad()
{
    double ret = 0;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        ret += it->second->getReactivePowerLoad();
    }

    return ret;
}

double EnergyBusSet::getActivePowerGen()
{
    double ret = 0;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        ret += it->second->getActivePowerGen();
    }

    return ret;
}

double EnergyBusSet::getReactivePowerGen()
{
    double ret = 0;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        ret += it->second->getReactivePowerGen();
    }

    return ret;
}

int EnergyBusSet::size()
{
    return buses.size();
}

bool EnergyBusSet::checkRestractions()
{
    bool ret = true;

    for (it = buses.begin(); it != buses.end(); ++it)
    {
        if ( ! it->second->checkRestractions())
        {
            ret = false;
            break;
        }
    }

    return ret;
}

EnergyBus* EnergyBusSet::operator [](int n)
{
    it = buses.begin();
    for (int i = 0; i < n; ++i, ++it);
    return it->second;
}
