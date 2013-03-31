#include "EnergyBus.h"

using namespace std;

EnergyBus::EnergyBus(int n, int c)
{
    no = n;
    code = c;
    angle = minAngle = maxAngle = 0;
    voltage = minVoltage = maxVoltage =  0;
    activePowerLoad = reactivePowerLoad = 0;
    reactivePowerGen = minReactivePowerGen = maxReactivePowerGen = 0;
    activePowerGen = minActivePowerGen = maxActivePowerGen = 0;
    G = B = 0;
}

int EnergyBus::getNo()
{
    return no;
}

double EnergyBus::cost()
{
    double ret = 0;

    if (this->isGeneratorBus())
    {
        double coefA = (*costCoefficents.begin())[2],
               coefB = (*costCoefficents.begin())[3],
               coefC = (*costCoefficents.begin())[4];

        for (vector <double *>::iterator it = costCoefficents.begin(); it != costCoefficents.end(); ++it)
        {
            if (activePowerGen > (*it)[0] && activePowerGen <= (*it)[1]) {
                coefA = (*it)[2];
                coefB = (*it)[3];
                coefC = (*it)[4];
                break;
            }
        }

        ret = coefA * activePowerGen * activePowerGen + coefB * activePowerGen + coefC;
    }

    return ret;
}

double EnergyBus::getVoltage()
{
    return voltage;
}

EnergyBus * EnergyBus::setVoltage(double value)
{
    voltage = value;
    return this;
}

EnergyBus * EnergyBus::setVoltage(double value, double minV, double maxV)
{
    voltage = value;
    minVoltage = minV;
    maxVoltage = maxV;
    return this;
}

double EnergyBus::getAngle()
{
    return angle;
}

EnergyBus * EnergyBus::setAngle(double value)
{
    angle = value;
    return this;
}

EnergyBus * EnergyBus::setAngle(double value, double minA, double maxA)
{
    angle = value;
    minAngle = minA;
    maxAngle = maxA;
    return this;
}

double EnergyBus::getActivePowerGen()
{
    return activePowerGen;
}

EnergyBus * EnergyBus::setActivePowerGen(double value)
{
    activePowerGen = value;
    return this;
}

double EnergyBus::getActivePowerLoad()
{
    return activePowerLoad;
}

EnergyBus * EnergyBus::setActivePowerLoad(double value)
{
    activePowerLoad = value;
    return this;
}

double EnergyBus::getReactivePowerGen()
{
    return reactivePowerGen;
}

EnergyBus * EnergyBus::setReactivePowerGen(double value)
{
    reactivePowerGen = value;
    return this;
}

double EnergyBus::getReactivePowerLoad()
{
    return reactivePowerLoad;
}

EnergyBus * EnergyBus::setReactivePowerLoad(double value)
{
    reactivePowerLoad = value;
    return this;
}

double * EnergyBus::getPowerGenLimits()
{
    double * ret = new double[4];
    ret[0] = minActivePowerGen;
    ret[1] = maxActivePowerGen;
    ret[2] = minReactivePowerGen;
    ret[3] = maxReactivePowerGen;
    return ret;
}

EnergyBus * EnergyBus::setPowerGenLimits(double minP, double maxP, double minQ, double maxQ)
{
    minActivePowerGen = minP;
    maxActivePowerGen = maxP;
    minReactivePowerGen = minQ;
    maxReactivePowerGen = maxQ;
    return this;
}

EnergyBus * EnergyBus::setCostPart(double minP, double maxP, float A, float B, float C)
{
    double * part = new double[5];
    part[0] = minP; part[1] = maxP;
    part[2] = A; part[3] = B; part[4] = C;
    costCoefficents.push_back(part);
    return this;
}

EnergyBus * EnergyBus::addLinkedBus(EnergyBus * bus, double g, double b)
{
    links.push_back(bus);
    G += g;
    B += b;
    return this;
}

vector <EnergyBus*> EnergyBus::getLinkedBus()
{
    return links;
}

double EnergyBus::getG()
{
    return -G;
}

double EnergyBus::getB()
{
    return -B;
}

bool EnergyBus::isGeneratorBus()
{
    return code == 1 || code == 2;
}

bool EnergyBus::isBalancedBus()
{
    return code == 1;
}

bool EnergyBus::checkRestractions()
{
    return activePowerGen <= maxActivePowerGen && activePowerGen >= minActivePowerGen
        && reactivePowerGen <= maxReactivePowerGen && reactivePowerGen >= minReactivePowerGen
        && voltage <= maxVoltage && voltage >= minVoltage
        && angle <= maxAngle && angle >= minAngle;
}

void EnergyBus::display()
{
    cout << endl << "*** Узел №" << no << " *** "
        << "G=" << getG() << "; B=" << getB()
        << "; V=" << voltage << "; Angle=" << angle
        << "; Pload=" << activePowerLoad << "; Qload=" << reactivePowerLoad;

    if (isGeneratorBus())
    {
        cout << "; Pgen=" << activePowerGen << "; Qgen=" << reactivePowerGen
            << "; cost=" << cost();
    }

    cout << "; restractions=" << checkRestractions();
}
