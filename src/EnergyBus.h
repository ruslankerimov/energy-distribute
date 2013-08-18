#ifndef ENERGY_BUS_INCLUDED
#define ENERGY_BUS_INCLUDED

#include <iostream>
#include <vector>
#include <map>

using namespace std;

class EnergyBus
{
private:
    int no, code;
    double angle, minAngle, maxAngle;
    double voltage, minVoltage, maxVoltage;
    double activePowerLoad, reactivePowerLoad;
    double reactivePowerGen, minReactivePowerGen, maxReactivePowerGen;
    double activePowerGen, minActivePowerGen, maxActivePowerGen;
    double G, B;
    double compensation, minCompensation, maxCompensation;
    vector <double*> costCoefficents;
    vector <EnergyBus*> links;

public:
    EnergyBus(int, int);

    int getNo();

    double cost();

    double getVoltage();

    EnergyBus * setVoltage(double value);

    EnergyBus * setVoltage(double value, double minV, double maxV);

    double getAngle();

    EnergyBus * setAngle(double value);

    EnergyBus * setAngle(double value, double minA, double maxA);

    double getActivePowerGen();

    EnergyBus * setActivePowerGen(double value);

    double getActivePowerLoad();

    EnergyBus * setActivePowerLoad(double);

    double getReactivePowerGen();

    EnergyBus * setReactivePowerGen(double);

    double getReactivePowerLoad();

    EnergyBus * setReactivePowerLoad(double);

    double getCompensation();

    EnergyBus * setCompensation(double);

    EnergyBus * setCompensationLimits(double, double);

    double * getCompensationLimits();

    double * getPowerGenLimits();

    EnergyBus * setPowerGenLimits(double minP, double maxP, double minQ, double maxQ);

    EnergyBus * setCostPart(double minP, double maxP, float A, float B, float C);

    EnergyBus * addLinkedBus(EnergyBus *, double, double);

    vector <EnergyBus*> getLinkedBus();

    double getG();

    double getB();

    bool isGeneratorBus();

    bool isBalancedBus();

    bool isWithCompensation;

    bool checkRestractions();

    void display();
};

#endif
