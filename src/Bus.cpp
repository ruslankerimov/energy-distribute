#include <iostream>
#include <vector>
#include <map>
#include "../lib/tinyxml/tinyxml.cpp"
#include "../lib/tinyxml/tinystr.cpp"
#include "../lib/tinyxml/tinyxmlerror.cpp"
#include "../lib/tinyxml/tinyxmlparser.cpp"

using namespace std;

class Bus
{
private:
    int no, code;
    double voltage, angle,
            activePowerLoad, activePowerGen,
            reactivePowerLoad, reactivePowerGen,
            minActivePowerGen, maxActivePowerGen,
            minReactivePowerGen, maxReactivePowerGen;
    double costCoefficents[3][5];

public:
    Bus(int n, int c)
    {
        no = n;
        code = c;
        angle = voltage = activePowerLoad = activePowerGen =
        reactivePowerLoad = reactivePowerGen = minActivePowerGen = maxActivePowerGen =
        minReactivePowerGen = maxReactivePowerGen = 0;
    }

    int getNo()
    {
        return no;
    }

    double cost()
    {
        float coefA = costCoefficents[2][2],
              coefB = costCoefficents[2][3],
              coefC = costCoefficents[2][4];

        for (int i = 0; i < 3; ++i)
        {
            if (activePowerLoad > costCoefficents[i][0] && activePowerLoad <= costCoefficents[i][1]) {
                coefA = costCoefficents[i][2];
                coefB = costCoefficents[i][3];
                coefC = costCoefficents[i][4];
                break;
            }
        }

        return coefA * activePowerLoad * activePowerLoad + coefB * activePowerLoad + coefC;
    }

    double getVoltage()
    {
        return voltage;
    }

    Bus * setVoltage(double value)
    {
        voltage = value;
        return this;
    }

    double getActivePowerGen()
    {
        return activePowerGen;
    }

    Bus * setActivePowerGen(double value)
    {
        activePowerGen = value;
        return this;
    }

    double getAngle()
    {
        return angle;
    }

    Bus * setAngle(double value)
    {
        angle = value;
        return this;
    }

    double * getLimits()
    {
        double * ret = new double[4];
        ret[0] = minActivePowerGen;
        ret[1] = maxActivePowerGen;
        ret[2] = minReactivePowerGen;
        ret[3] = maxReactivePowerGen;
        return ret;
    }

    Bus * setLimits(double minP, double maxP, double minQ, double maxQ)
    {
        minActivePowerGen = minP;
        maxActivePowerGen = maxP;
        minReactivePowerGen = minQ;
        maxReactivePowerGen = maxQ;
        return this;
    }

    bool isGeneratorBus()
    {
        return code == 1 || code == 2;
    }

    bool isBalancedBus()
    {
        return code == 1;
    }
};

class BusCollection
{
private:
    map <int, Bus *> buses;
    map <int, Bus *>::iterator it;
public:
    BusCollection()
    {

    }

    Bus * getBus(int n)
    {
        return buses[n];
    }

    BusCollection * addBus(Bus * bus)
    {
        buses[bus->getNo()] = bus;
        return this;
    }

    double cost()
    {
        double ret = 0;

        for (it = buses.begin(); it != buses.end(); ++it)
        {
            ret += it->second->cost();
        }

        return ret;
    }
};

class Line
{
private:
public:
};


void fillData()
{
    TiXmlDocument busData("../data/set_default/bus_data.xml");
    busData.LoadFile();
    TiXmlNode * buses = busData.FirstChild("bus-data")->FirstChild("buses");

    cout << buses->LastChild("bus")->FirstChild("no")->FirstChild()->Value();
}

int main()
{

    fillData();
    return 0;
}
