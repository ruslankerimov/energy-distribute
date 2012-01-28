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
    vector <double*> costCoefficents;
    vector <EnergyBus*> links;

public:
    EnergyBus(int n, int c)
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

    EnergyBus * setVoltage(double value)
    {
        voltage = value;
        return this;
    }

    EnergyBus * setVoltage(double value, double minV, double maxV)
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

    EnergyBus * setAngle(double value)
    {
        angle = value;
        return this;
    }

    EnergyBus * setAngle(double value, double minA, double maxA)
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

    EnergyBus * setActivePowerGen(double value)
    {
        activePowerGen = value;
        return this;
    }

    double getActivePowerLoad()
    {
        return activePowerLoad;
    }

    EnergyBus * setActivePowerLoad(double value)
    {
        activePowerLoad = value;
        return this;
    }

    double getReactivePowerGen()
    {
        return reactivePowerGen;
    }

    EnergyBus * setReactivePowerGen(double value)
    {
        reactivePowerGen = value;
        return this;
    }

    double getReactivePowerLoad()
    {
        return reactivePowerLoad;
    }

    EnergyBus * setReactivePowerLoad(double value)
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

    EnergyBus * setPowerGenLimits(double minP, double maxP, double minQ, double maxQ)
    {
        minActivePowerGen = minP;
        maxActivePowerGen = maxP;
        minReactivePowerGen = minQ;
        maxReactivePowerGen = maxQ;
        return this;
    }

    EnergyBus * setCostPart(double minP, double maxP, float A, float B, float C)
    {
        double * part = new double[5];
        part[0] = minP; part[1] = maxP;
        part[2] = A; part[3] = B; part[4] = C;
        costCoefficents.push_back(part);
        return this;
    }

    EnergyBus * addLinkedBus(EnergyBus * bus, double g, double b)
    {
        links.push_back(bus);
        G += g;
        B += b;
        return this;
    }

    vector <EnergyBus*> getLinkedBus()
    {
        return links;
    }

    double getG()
    {
        return -G;
    }

    double getB()
    {
        return -B;
    }

    bool isGeneratorBus()
    {
        return code == 1 || code == 2;
    }

    bool isBalancedBus()
    {
        return code == 1;
    }

    bool checkRestractions()
    {
        return activePowerGen <= maxActivePowerGen && activePowerGen >= minActivePowerGen
            && reactivePowerGen <= maxReactivePowerGen && reactivePowerGen >= minReactivePowerGen
            && voltage <= maxVoltage && voltage >= minVoltage
            && angle <= maxAngle && angle >= minAngle;
    }
};

class EnergyBusSet
{
private:
    map <int, EnergyBus *> buses;
    map <int, EnergyBus *>::iterator it;
public:
    EnergyBusSet()
    {

    }

    EnergyBus* getBus(int n)
    {
        return buses[n];
    }

    EnergyBusSet* addBus(EnergyBus * bus)
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

    double getActivePowerLoad()
    {
        double ret = 0;

        for (it = buses.begin(); it != buses.end(); ++it)
        {
            ret += it->second->getActivePowerLoad();
        }

        return ret;
    }

    double getActivePowerGen()
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

    EnergyBus* get(int n)
    {
        it = buses.begin();
        for (int i = 0; i < n; ++i, ++it);
        return it->second;
    }

    bool checkRestractions()
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

    EnergyBus* operator [](int n)
    {
        it = buses.begin();
        for (int i = 0; i < n; ++i, ++it);
        return it->second;
    }
};

class EnergyLine
{
private:
    EnergyBus * from;
    EnergyBus * to;
    float R, X;
    double G, B;
public:
    EnergyLine(EnergyBus * fromBus, EnergyBus * toBus, double r, double x)
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

    EnergyBus * getFrom()
    {
        return from;
    }

    EnergyBus * getTo()
    {
        return to;
    }
};

class EnergyLineSet
{
private:
    map <int, EnergyLine *> lines;
    map <int, vector <EnergyLine*> > links;
    map <int, EnergyLine *>::iterator it;
public:
    EnergyLineSet()
    {

    }

    EnergyLineSet * addLine(EnergyLine * line)
    {
        // @todo правильно составить ключ
        int key = line->getFrom()->getNo() * 1000 + line->getTo()->getNo();
        lines[key] = line;
        //@todo
        links[line->getFrom()->getNo()].push_back(line);
        links[line->getTo()->getNo()].push_back(line);
        line->getFrom()->addLinkedBus(line->getTo(), line->getG(), line->getB());
        line->getTo()->addLinkedBus(line->getFrom(), line->getG(), line->getB());
        return this;
    }

    EnergyLine * getLine(int from, int to)
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

    vector <EnergyLine*> getBusLines(int busNo)
    {
        return links[busNo];
    }

    EnergyLine* get(int n)
    {
        it = lines.begin();
        for (int i = 0; i < n; ++i, ++it);
        return it->second;
    }

    EnergyLine* operator [](int n)
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
