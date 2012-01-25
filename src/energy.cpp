#include <iostream>
#include <vector>
#include <ga/ga.h>
#include "../lib/tinyxml/tinyxml.cpp"
#include "../lib/tinyxml/tinystr.cpp"
#include "../lib/tinyxml/tinyxmlerror.cpp"
#include "../lib/tinyxml/tinyxmlparser.cpp"

#include "newton.cpp"
#include "Bus.cpp"

#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>

using namespace std;

void parseBusData();
void parseLinesData();
void parseGAparams();
float objective(GAGenome &);
void calculate(GARealGenome, bool);
double getYCell(int, int, vector <double>);
double getFCell(int, vector <double>);
void calculation(GARealGenome);
double getLosses();

Bus * balanceBus;
LineCollection lines;
BusCollection allBus;
BusCollection genBus;
BusCollection notGenBus;
BusCollection genWithoutBalanceBus;
GAParameterList params;

int main(int argc, char** argv)
{
    parseBusData();
    parseLinesData();
    parseGAparams();
    params.parse(argc, argv, gaFalse);

    cout << endl << "*** Исходные данные ***";
    cout << endl << "Генераторные узлы: ";
    for (int i = 0; i < genBus.size(); ++i) {
        cout << genBus.get(i)->getNo() << " ";
    }
    cout << endl << "Нагрузочные узлы: ";
    for (int i = 0; i < notGenBus.size(); ++i) {
        cout << notGenBus.get(i)->getNo() << " ";
    }
    cout << endl << "Балансирующий узел: " << balanceBus->getNo();
    cout << endl << "Линии и из характеристики: ";
    for (int i = 0; i < lines.size(); ++i) {
        Line * line = lines.get(i);
        cout << endl << "из " << line->getFrom()->getNo() << " в " << line->getTo()->getNo()
                << "; G=" << line->getG() << ", B=" << line->getB();
    }
    cout << endl << "Характеристики узлов: ";
    for (int i = 0; i < allBus.size(); ++i) {
        Bus * bus = allBus.get(i);
        cout << endl << "Узел №" << bus->getNo()
                << ": Gkk=" << bus->getG() << "; Bkk=" << bus->getB()
                << "; Pload=" << bus->getActivePowerLoad() << "; Qload=" << bus->getReactivePowerLoad();
    }



//    n = nodes.size();
//    k = genNodes.size();
//

    GARealAlleleSetArray alleles;
    cout << endl << "Лимиты генераторных узлов: ";
    for (int i = 0; i < genBus.size(); ++i) {
        Bus * bus = genBus.get(i);
        if (bus->isBalancedBus())
        {
            continue;
        }
        double * powerLimits = bus->getPowerGenLimits();
        cout << endl << "узел №" << bus->getNo() << " : " << powerLimits[0] << " - " << powerLimits[1] << " МВт";
        alleles.add(powerLimits[0], powerLimits[1], GAAllele::INCLUSIVE, GAAllele::INCLUSIVE);
    }

    cout << endl;

    GARealGenome genome(alleles, objective);
    genome.crossover(GARealGenome::OnePointCrossover);
    GASteadyStateGA ga(genome);
    ga.parameters(params);
    GANoScaling scaling;
    ga.scaling(scaling);
    GATournamentSelector selector;
    ga.selector(selector);
    ga.evolve();

    cout << endl << "*** Результаты ***";
    cout << endl << "Нужная активная мощность: " << allBus.activePowerLoad() << " МВт";

    genome = ga.statistics().bestIndividual();
    calculate(genome, true);
    cout << endl << "Получившиеся мощности: ";
    for (int i = 0; i < genBus.size(); ++i) {
        Bus * bus = genBus.get(i);
        cout << endl << "узел №" << bus->getNo() << " " << bus->getActivePowerGen() << " МВт";
    }

    cout << endl << "Получившаяся суммарная мощность: " << allBus.activePowerGen() << " МВт";
    cout << endl << "Получившаяся стоимость: " << allBus.cost();

    cout << endl << endl;

    return 0;
}

float objective(GAGenome & g) {
    GARealGenome& genome = (GARealGenome&) g;
    double cost = allBus.cost();
    calculate(genome, false);
    return 1 / allBus.cost();
}

void calculate(GARealGenome genome, bool flag)
{
    for (int i = 0, size = genWithoutBalanceBus.size(); i < size; ++i)
    {
        Bus* bus = genWithoutBalanceBus.get(i);
        bus->setActivePowerGen(genome.gene(i));
    }

    vector <double> x;
    int n = allBus.size();
    // начальные значения
    for (int i = 0; i < n * 2; ++i) {
        x.push_back(i < n ? 0 : 1);
    }


    int solve = flag ? newton(x, getFCell, getYCell) : 1;
    if (flag)
    {
        cout << endl << "Решение: " << solve;
    }

    if (solve)
    {
        for (int i = 0; i < n; ++i) {
            Bus* bus = allBus.get(i);

            bus->setAngle(x[i])->setVoltage(x[n + i]);
        }
    }

    double losses = getLosses();
    double power = allBus.activePowerLoad() + losses - genWithoutBalanceBus.activePowerGen();
    power < 0 && (power = 0);
    balanceBus->setActivePowerGen(power);
//
//    cout << endl << "Потери: " << losses;
//    cout << endl << "Мощность в балансирующем узле: " << power;
//    cout << endl;
}

double getYCell(int i, int j, vector <double> x) {
    double ret = 0;

    int n = allBus.size();
    int k = i % n;
    int m = j % n;
    Bus* kBus = allBus.get(k);
    Bus* mBus = allBus.get(m);
    Line* kmLine = lines.getLine(kBus->getNo(), mBus->getNo());

    bool isForP = i < n;
    bool isDelta = j < n;
    bool isGen = kBus->isGeneratorBus();

    double Vk = x[k + n];
    double Vm = x[m + n];
    double Dk = x[k];
    double Dm = x[m];
    double Bkk = kBus->getB();
    double Gkk = kBus->getG();
    double Bkm = kmLine ? kmLine->getB() : 0;
    double Gkm = kmLine ? kmLine->getG() : 0;

//  cout << endl << "DEBUG: i=" << i << ", j=" << j
//          << ", kNodeNumber=" << kBus->getNo()
//          << ", mNodeNumber=" << mBus->getNo()
//          << endl
//          << ", isGen=" << isGen
//          << ", isForP=" << isForP
//          << ", isDelta=" << isDelta
//          << ", Vk=" << Vk
//          << ", Vm=" << Vm
//          << endl
//          << ", Dk=" << Dk
//          << ", Dm=" << Dm
//          << ", Gkm=" << Gkm << ", Bkm=" << Bkm
//          << ", Gkk=" << Gkk << ", Bkk=" << Bkk
//          << endl;

    if (kBus->isBalancedBus()) {
        if (i == j) {
            ret = 1;
        } else {
            ret = 0;
        }

        return ret;
    } else if (isGen && ! isForP) {
        if (i == j) {
            ret = 1;
        } else {
            ret = 0;
        }

        return ret;
    }

    if (isForP) {
        if (isDelta) {
            if (kBus == mBus) {
                vector <Line*> busLines = lines.getBusLines(kBus->getNo());
                for (vector <Line*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
                    Line * line = *it;
                    Bus * from = line->getFrom();
                    Bus * to = line->getTo();
                    double Bkm = line->getB();
                    double Gkm = line->getG();
                    double Dm, Vm;

                    if (to == kBus)
                    {
                        to = from;
                    }
                    Dm = x[to->getNo() - 1]; // @todo;
                    Vm = x[n + to->getNo() -1];

                    ret += Vm * (Gkm * sin(Dk - Dm) - Bkm * cos(Dk - Dm));
                }
                ret *= Vk;
            } else {
                ret = Vk * Vm
                        * (-Gkm * sin(Dk - Dm) + Bkm * cos(Dk - Dm));
            }
        } else {
            if (kBus == mBus) {
                ret -= 2 * Vk * Gkk;
                vector <Line*> busLines = lines.getBusLines(kBus->getNo());
                for (vector <Line*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
                    Line * line = *it;
                    Bus * from = line->getFrom();
                    Bus * to = line->getTo();
                    double Bkm = line->getB();
                    double Gkm = line->getG();
                    double Dm, Vm;

                    if (to == kBus)
                    {
                        to = from;
                    }
                    Dm = x[to->getNo() - 1]; // @todo;
                    Vm = x[n + to->getNo() -1];

                    ret -= Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
                }
            } else {
                ret = -Vk
                        * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
            }
        }
    } else {
        if (isDelta) {
            if (kBus == mBus) {
                vector <Line*> busLines = lines.getBusLines(kBus->getNo());
                for (vector <Line*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
                    Line * line = *it;
                    Bus * from = line->getFrom();
                    Bus * to = line->getTo();
                    double Bkm = line->getB();
                    double Gkm = line->getG();
                    double Dm, Vm;

                    if (to == kBus)
                    {
                        to = from;
                    }
                    Dm = x[to->getNo() - 1]; // @todo;
                    Vm = x[n + to->getNo() -1];

                    ret -= Vm * (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
                }
                ret *= Vk;
            } else {
                ret = Vk * Vm
                        * (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
            }
        } else {
            if (kBus == mBus) {
                ret += 2 * Vk * Bkk;
                vector <Line*> busLines = lines.getBusLines(kBus->getNo());
                for (vector <Line*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
                    Line * line = *it;
                    Bus * from = line->getFrom();
                    Bus * to = line->getTo();
                    double Bkm = line->getB();
                    double Gkm = line->getG();
                    double Dm, Vm;

                    if (to == kBus)
                    {
                        to = from;
                    }
                    Dm = x[to->getNo() - 1]; // @todo;
                    Vm = x[n + to->getNo() -1];

                    ret += Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
                }
            } else {
                ret = Vk
                        * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
            }
        }
    }

    return ret;
}

double getFCell(int i, vector <double> x) {
    double ret = 0;
    int n = allBus.size();
    int j = i % n;
    Bus* bus = allBus.get(j);
    bool isForP = i < n;

    double Vk = x[j + n];
    double Pk = (-bus->getActivePowerLoad() + bus->getActivePowerGen()) / 100;
    double Qk = bus->getReactivePowerLoad() / 100;
    double Gkk = bus->getG();
    double Bkk = bus->getB();
    double Dk = x[j];

    if (bus->isBalancedBus()) {
        if (isForP) {
            ret = Dk;
        } else {
            ret = Vk - bus->getVoltage();
        }
    } else if (bus->isGeneratorBus() && ! isForP) {
        ret = Vk - bus->getVoltage();
    } else {
        if (isForP) {
            ret += Pk - Vk * Vk * Gkk;
        } else {
            ret += Qk + Vk * Vk * Bkk;
        }

        vector <Line*> busLines = lines.getBusLines(bus->getNo());
        for (vector <Line*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
            Line * line = *it;
            Bus * from = line->getFrom();
            Bus * to = line->getTo();
            double Bkm = line->getB();
            double Gkm = line->getG();
            double Dm, Vm;

            if (to == bus)
            {
                to = from;
            }
            Dm = x[to->getNo() - 1]; // @todo;
            Vm = x[n + to->getNo() -1];

            if (isForP) {
                ret -= Vk * Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
            } else {
                ret += Vk * Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
            }
        }
    }

    return ret;
}




double getLosses()
{
    double ret = 0;

    for (int i = 0, size = lines.size(); i < size; ++i)
    {
        Line* line = lines.get(i);
        double Vk = line->getFrom()->getAngle();
        double Vm = line->getTo()->getVoltage();
        double Dk = line->getFrom()->getAngle();
        double Dm = line->getTo()->getAngle();
        double Gkm = line->getG();

        ret += Gkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dk - Dm));
    }

    return 0;
}

void parseBusData()
{
    TiXmlDocument busDataFile("input/set_default/bus_data.xml");
    busDataFile.LoadFile();
    TiXmlElement * busElements = busDataFile.FirstChildElement("bus-data")->FirstChildElement("buses");
    TiXmlElement * busElement = busElements->FirstChildElement("bus");

    do
    {
        int no = atoi(busElement->FirstChildElement("no")->GetText());
        int code = atoi(busElement->FirstChildElement("code")->GetText());

        Bus * bus = new Bus(no, code);

        bus->setVoltage(
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("value")->GetText()),
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("min-value")->GetText()),
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("max-value")->GetText())
        )->setAngle(
            atof(busElement->FirstChildElement("angle")->FirstChildElement("value")->GetText()),
            atof(busElement->FirstChildElement("angle")->FirstChildElement("min-value")->GetText()),
            atof(busElement->FirstChildElement("angle")->FirstChildElement("max-value")->GetText())
        )->setActivePowerLoad(
            atof(busElement->FirstChildElement("power-load")->FirstChildElement("active")->FirstChildElement("value")->GetText())
        )->setReactivePowerLoad(
            atof(busElement->FirstChildElement("power-load")->FirstChildElement("reactive")->FirstChildElement("value")->GetText())
        );

        if (bus->isGeneratorBus())
        {
            bus->setActivePowerGen(
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("active")->FirstChildElement("value")->GetText())
            )->setReactivePowerGen(
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("reactive")->FirstChildElement("value")->GetText())
            )->setPowerGenLimits(
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("active")->FirstChildElement("min-value")->GetText()),
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("active")->FirstChildElement("max-value")->GetText()),
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("reactive")->FirstChildElement("min-value")->GetText()),
                atof(busElement->FirstChildElement("power-gen")->FirstChildElement("reactive")->FirstChildElement("max-value")->GetText())
            );

            TiXmlElement * costPartElement = busElement->FirstChildElement("cost")->FirstChildElement("part");
            do
            {
                bus->setCostPart(
                    atof(costPartElement->FirstChildElement("power")->FirstChildElement("min-value")->GetText()),
                    atof(costPartElement->FirstChildElement("power")->FirstChildElement("max-value")->GetText()),
                    atof(costPartElement->FirstChildElement("coefs")->FirstChildElement("A")->GetText()),
                    atof(costPartElement->FirstChildElement("coefs")->FirstChildElement("B")->GetText()),
                    atof(costPartElement->FirstChildElement("coefs")->FirstChildElement("C")->GetText())
                );
            }
            while (costPartElement = costPartElement->NextSiblingElement("part"));
            genBus.addBus(bus);
            if ( ! bus->isBalancedBus())
            {
                genWithoutBalanceBus.addBus(bus);
            }
        }
        else
        {
            notGenBus.addBus(bus);
        }

        if (bus->isBalancedBus())
        {
            balanceBus = bus;
        }

        allBus.addBus(bus);
    }
    while (busElement = busElement->NextSiblingElement("bus"));
}

void parseLinesData()
{
    TiXmlDocument lineDataFile("input/set_default/lines_data.xml");
    lineDataFile.LoadFile();
    TiXmlElement * linesElement = lineDataFile.FirstChildElement("lines-data")
                                                ->FirstChildElement("lines");
    TiXmlElement * lineElement = linesElement->FirstChildElement("line");

    do
    {
        int from = atoi(lineElement->FirstChildElement("bus-from")->GetText());
        int to   = atoi(lineElement->FirstChildElement("bus-to")->GetText());

        Bus * busFrom = allBus.getBus(from);
        Bus * busTo = allBus.getBus(to);

        double R = atof(lineElement->FirstChildElement("resistance")->FirstChildElement("active")->GetText());
        double X = atof(lineElement->FirstChildElement("resistance")->FirstChildElement("reactive")->GetText());

        Line * line = new Line(busFrom, busTo, R, X);
        lines.addLine(line);
    }
    while (lineElement = lineElement->NextSiblingElement("line"));

}

void parseGAparams()
{
    GASteadyStateGA::registerDefaultParameters(params);
    TiXmlDocument gaParamsFile("input/set_default/ga_params.xml");
    gaParamsFile.LoadFile();
    TiXmlElement * gaParamsElement = gaParamsFile.FirstChildElement("ga-params");

    params.set(gaNnGenerations,   atoi(gaParamsElement->FirstChildElement("number-of-generation")->GetText()));
    params.set(gaNpopulationSize, atoi(gaParamsElement->FirstChildElement("population-size")->GetText()));
    params.set(gaNpMutation,      atof(gaParamsElement->FirstChildElement("mutation-probability")->GetText()));
    params.set(gaNpCrossover,     atof(gaParamsElement->FirstChildElement("crossover-probability")->GetText()));

    params.set(gaNscoreFrequency, atoi(gaParamsElement->FirstChildElement("score-frequency")->GetText()));
    params.set(gaNflushFrequency, atoi(gaParamsElement->FirstChildElement("score-flush-frequency")->GetText()));
    params.set(gaNselectScores,   (int)GAStatistics::AllScores);
    params.set(gaNscoreFilename,  gaParamsElement->FirstChildElement("score-filename")->GetText());
}
