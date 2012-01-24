#include <iostream>
#include <vector>
#include <map>

using namespace std;

class Bus
{
private:
    int no, code;
    double angle, minAngle, maxAngle;
    double voltage, minVoltage, maxVoltage;
    double activePowerLoad, reactivePowerLoad;
    double reactivePowerGen, minReactivePowerGen, maxReactivePowerGen;
    double activePowerGen, minActivePowerGen, maxActivePowerGen;
    vector <double *> costCoefficents;
    map <int, Bus *> links;

public:
    Bus(int n, int c)
    {
        no = n;
        code = c;
        angle = minAngle = maxAngle = 0;
        voltage = minVoltage = maxVoltage =  0;
        activePowerLoad = reactivePowerLoad = 0;
        reactivePowerGen = minReactivePowerGen = maxReactivePowerGen = 0;
        activePowerGen = minActivePowerGen = maxActivePowerGen = 0;
    }

    int getNo()
    {
        return no;
    }

    double cost()
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

    double getVoltage()
    {
        return voltage;
    }

    Bus * setVoltage(double value)
    {
        voltage = value;
        return this;
    }

    Bus * setVoltage(double value, double minV, double maxV)
    {
        voltage = value;
        minVoltage = minV;
        maxVoltage = maxV;
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

    Bus * setAngle(double value, double minA, double maxA)
    {
        angle = value;
        minAngle = minA;
        maxAngle = maxA;
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

    double getActivePowerLoad()
    {
        return activePowerLoad;
    }

    Bus * setActivePowerLoad(double value)
    {
        activePowerLoad = value;
        return this;
    }

    double getReactivePowerGen()
    {
        return reactivePowerGen;
    }

    Bus * setReactivePowerGen(double value)
    {
        reactivePowerGen = value;
        return this;
    }

    double getReactivePowerLoad()
    {
        return reactivePowerLoad;
    }

    Bus * setReactivePowerLoad(double value)
    {
        reactivePowerLoad = value;
        return this;
    }

    double * getPowerGenLimits()
    {
        double * ret = new double[4];
        ret[0] = minActivePowerGen;
        ret[1] = maxActivePowerGen;
        ret[2] = minReactivePowerGen;
        ret[3] = maxReactivePowerGen;
        return ret;
    }

    Bus * setPowerGenLimits(double minP, double maxP, double minQ, double maxQ)
    {
        minActivePowerGen = minP;
        maxActivePowerGen = maxP;
        minReactivePowerGen = minQ;
        maxReactivePowerGen = maxQ;
        return this;
    }

    Bus * setCostPart(double minP, double maxP, float A, float B, float C)
    {
        double * part = new double[5];
        part[0] = minP; part[1] = maxP;
        part[2] = A; part[3] = B; part[4] = C;
        costCoefficents.push_back(part);
        return this;
    }

    Bus * addLinkedBus(Bus * bus)
    {
        links[bus->getNo()] = bus;
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

    double activePowerLoad()
    {
        double ret = 0;

        for (it = buses.begin(); it != buses.end(); ++it)
        {
            ret += it->second->getActivePowerLoad();
        }

        return ret;
    }

    double activePowerGen()
    {
        double ret = 0;

        for (it = buses.begin(); it != buses.end(); ++it)
        {
            ret += it->second->getActivePowerGen();
        }

        return ret;
    }

    int size()
    {
        return buses.size();
    }

    // @todo rename, refactor, operator []
    Bus * get(int n)
    {
        it = buses.begin();
        for (int i = 0; i < n; ++i, ++it);
        return it->second;
    }
};

class Line
{
private:
    Bus * from;
    Bus * to;
    float R, X;
    double G, B;
public:
    Line(Bus * fromBus, Bus * toBus, double r, double x)
    {
        from = fromBus;
        to   = toBus;
        R = r;
        X = x;
        G = R / (X * X + R * R);
        B = X / (X * X + R * R);
    }

    double getR()
    {
        return R;
    }

    double getX()
    {
        return X;
    }

    double getG()
    {
        return G;
    }

    double getB()
    {
        return B;
    }

    Bus * getFrom()
    {
        return from;
    }

    Bus * getTo()
    {
        return to;
    }
};

class LineCollection
{
private:
    map <int, Line *> lines;
    map <int, Line *>::iterator it;
public:
    LineCollection()
    {

    }

    LineCollection * addLine(Line * line)
    {
        // @todo правильно составить ключ
        int key = line->getFrom()->getNo() * 1000 + line->getTo()->getNo();
        lines[key] = line;
        return this;
    }

    Line * getLine(int from, int to)
    {
        int key = from * 1000 + to;
        if (lines.count(key) < 1)
        {
            key = to * 1000 + from;
        }
        // @todo узкое место, нужна проверка на map::end()
        return lines.find(key)->second;
    }

    int size()
    {
        return lines.size();
    }

    // @todo rename, refactor, operator []
    Line * get(int n)
    {
        it = lines.begin();
        for (int i = 0; i < n; ++i, ++it);
        return it->second;
    }
};



//int main()
//{
//
//    fillBusData();
//    fillLineData();
//    return 0;
//}
