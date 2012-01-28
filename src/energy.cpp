#include <iostream>
#include <vector>
#include <ga/ga.h>

#include "../lib/tinyxml/tinyxml.cpp"
#include "../lib/tinyxml/tinystr.cpp"
#include "../lib/tinyxml/tinyxmlerror.cpp"
#include "../lib/tinyxml/tinyxmlparser.cpp"

#include "newton.cpp"
#include "EnergyBus.cpp"

#define INSTANTIATE_REAL_GENOME
#include <ga/GARealGenome.h>

using namespace std;

void parseBusData();
void parseLinesData();
void parseGAparams();
float objective(GAGenome &);
void calculate(GARealGenome, bool);
void fillFromGenome(GARealGenome);
double getYCell(int, int, vector <double>);
double getFCell(int, vector <double>);
void calculation(GARealGenome);
double getLosses();

const long double PI = 2 * asin(1);

EnergyBus * balanceBus;
EnergyLineSet lines;
EnergyBusSet allBus;
EnergyBusSet genBus;
EnergyBusSet notGenBus;
EnergyBusSet genWithoutBalanceBus;
GAParameterList params;

int main(int argc, char** argv)
{
    parseBusData();
    parseLinesData();
    parseGAparams();
    params.parse(argc, argv, gaFalse);

    vector <EnergyLine*> busLines = lines.getBusLines(6);
    for (vector <EnergyLine*>::iterator it = busLines.begin(); it != busLines.end(); ++it)
    {
        EnergyLine* line = *it;
        cout << endl << line->getFrom()->getNo() << "->" << line->getTo()->getNo();
    }

    cout << endl << "*** Исходные данные ***";
    cout << endl << "Генераторные узлы: ";
    for (int i = 0; i < genBus.size(); ++i) {
        cout << genBus[i]->getNo() << " ";
    }
    cout << endl << "Нагрузочные узлы: ";
    for (int i = 0; i < notGenBus.size(); ++i) {
        cout << notGenBus[i]->getNo() << " ";
    }
    cout << endl << "Балансирующий узел: " << balanceBus->getNo();
    cout << endl << "Линии и из характеристики: ";
    for (int i = 0; i < lines.size(); ++i) {
        EnergyLine * line = lines[i];
        cout << endl << "из " << line->getFrom()->getNo() << " в " << line->getTo()->getNo()
                << "; G=" << line->getG() << ", B=" << line->getB();
    }
    cout << endl << "Характеристики узлов: ";
    for (int i = 0; i < allBus.size(); ++i) {
        EnergyBus * bus = allBus[i];
        cout << endl << "Узел №" << bus->getNo()
                << ": G=" << bus->getG() << "; B=" << bus->getB() << "; V=" << bus->getVoltage()
                << "; Pload=" << bus->getActivePowerLoad() << "; Qload=" << bus->getReactivePowerLoad();
    }

    GARealAlleleSetArray alleles;
    cout << endl << "Лимиты генераторных узлов: ";
    for (int i = 0; i < genBus.size(); ++i) {
        EnergyBus * bus = genBus[i];
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
    cout << endl << "Нужная активная мощность: " << allBus.getActivePowerLoad() << " МВт";

    genome = ga.statistics().bestIndividual();
    fillFromGenome(genome);
    calculate(genome, true);
    cout << endl << "Получившиеся мощности: ";
    for (int i = 0; i < genBus.size(); ++i) {
        EnergyBus * bus = genBus[i];
        cout << endl << "узел №" << bus->getNo() << " " << bus->getActivePowerGen() << " МВт";
    }

    cout << endl << "Получившаяся суммарная мощность: " << allBus.getActivePowerGen() << " МВт";
    cout << endl << "Получившаяся стоимость: " << allBus.cost();

    for (int i = 0; i < allBus.size(); ++i) {
        EnergyBus * bus = allBus[i];
        bool check = bus->checkRestractions();
        cout << endl << "узел №" << bus->getNo() << " " << check;
        if ( ! check)
        {
            double* limits = bus->getPowerGenLimits();
            cout << endl << bus->getVoltage() << " " << bus->getAngle() << " " << limits[2] << " "<< limits[3]
              << (bus->getReactivePowerGen() >= limits[2]) << " "<< (bus->getReactivePowerGen() <= limits[3]);
        }

    }

    cout << endl << "Ограничения: " << allBus.checkRestractions();
    cout << endl;

    return 0;
}

float objective(GAGenome & g) {
    float ret = 0.0;
    GARealGenome& genome = (GARealGenome&) g;

    fillFromGenome(genome);
    double power = allBus.getActivePowerLoad() - genWithoutBalanceBus.getActivePowerGen();
    double* limits = balanceBus->getPowerGenLimits();

    //    Первая прикидка
    if (power > limits[1])
    {
        cout << endl << "here1" << " " << power << " " << limits[1];
        return limits[1] - power;
    }

    calculate(genome, true);
    double cost = allBus.cost();

    if (allBus.checkRestractions())
    {
        cost += 100;
    }

    ret = 1 / cost;

    return ret;
}

void fillFromGenome(GARealGenome genome)
{
    for (int i = 0, size = genWithoutBalanceBus.size(); i < size; ++i)
    {
        EnergyBus* bus = genWithoutBalanceBus[i];
        bus->setActivePowerGen(genome.gene(i));
    }
}

void calculate(GARealGenome genome, bool flag)
{
    vector <double> x;
    int n = allBus.size();
    // начальные значения
    for (int i = 0; i < n * 2; ++i) {
        x.push_back(i < n ? 0 : 1);
    }

    int solve = 0;
    if (flag)
    {
        solve = newton(x, getFCell, getYCell);
//        getYCell(5, 5, x);
//        cout << endl << "Решение: " << solve;
    }

    double losses = 0.0;
    if (solve)
    {
        for (int i = 0; i < n; ++i) {
            EnergyBus* bus = allBus[i];

            bus->setAngle(x[i])->setVoltage(x[n + i]);
        }

        losses = getLosses();
    }


    double power = allBus.getActivePowerLoad() + losses - genWithoutBalanceBus.getActivePowerGen();
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
    EnergyBus* kBus = allBus[k];
    EnergyBus* mBus = allBus[m];
    EnergyLine* kmLine = lines.getLine(kBus->getNo(), mBus->getNo());

    bool isForP = i < n;
    bool isDelta = j < n;
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
//          << ", isGen=" << kBus->isGeneratorBus()
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

    if (kBus->isBalancedBus())
    {
        if (i == j) {
            ret = 1;
        } else {
            ret = 0;
        }

        return ret;
    }
    else if (kBus->isGeneratorBus() && ! isForP)
    {
        if (i == j) {
            ret = 1;
        } else {
            ret = 0;
        }

        return ret;
    }

    if (kBus == mBus)
    {
        if ( ! isDelta)
        {
            if (isForP)
            {
                ret -= 2 * Vk * Gkk;
            }
            else
            {
                ret += 2 * Vk * Bkk;
            }
        }

        vector <EnergyLine*> busLines = lines.getBusLines(kBus->getNo());

        for (vector <EnergyLine*>::iterator it = busLines.begin(); it != busLines.end(); ++it)
        {
            EnergyLine* line = *it;
            EnergyBus* from = line->getFrom();
            EnergyBus* to = line->getTo();
            double Bkm = line->getB();
            double Gkm = line->getG();
            double Dm, Vm;

            if (to == kBus)
            {
                to = from;
            }

            Dm = x[to->getNo() - 1]; // @todo;
            Vm = x[n + to->getNo() -1];

            if (isForP)
            {
                if (isDelta)
                {
                    ret += Vk * Vm * (Gkm * sin(Dk - Dm) - Bkm * cos(Dk - Dm));
                }
                else
                {
                    ret -= Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
                }
            }
            else
            {
                if (isDelta)
                {
                    ret -= Vk * Vm * (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
                }
                else
                {
                    ret += Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
                }
            }
        }
    }
    else
    {
        if (isForP)
        {
            if (isDelta)
            {
                ret = Vk * Vm
                        * (-Gkm * sin(Dk - Dm) + Bkm * cos(Dk - Dm));
            }
            else
            {
                ret = -Vk
                        * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
            }
        }
        else
        {
            if (isDelta)
            {
                ret = Vk * Vm
                        * (Bkm * sin(Dk - Dm) + Gkm * cos(Dk - Dm));
            }
            else
            {
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
    int k = i % n;
    EnergyBus* kBus = allBus[k];
    bool isForP = i < n;

    double Vk = x[k + n];
    double Pk = (-kBus->getActivePowerLoad()) / 100;
    double Qk = kBus->getReactivePowerLoad() / 100;
    double Gkk = kBus->getG();
    double Bkk = kBus->getB();
    double Dk = x[k];

    if (kBus->isGeneratorBus())
    {
        Pk += kBus->getActivePowerGen() / 100;
    }

//  cout << endl << "DEBUG: j=" << j
//          << ", kNodeNumber=" << bus->getNo()
//          << endl
//          << ", isForP=" << isForP
//          << ", Vk=" << Vk
//          << endl
//          << ", Dk=" << Dk
//          << ", Gkk=" << Gkk << ", Bkk=" << Bkk
//          << endl;

    if (kBus->isBalancedBus()) {
        if (isForP) {
            ret = Dk;
        } else {
            ret = Vk - kBus->getVoltage();
        }
    } else if (kBus->isGeneratorBus() && ! isForP) {
        ret = Vk - kBus->getVoltage();
    } else {
        if (isForP) {
            ret += Pk - Vk * Vk * Gkk;
        } else {
            ret += Qk + Vk * Vk * Bkk;
        }

        vector <EnergyLine*> busLines = lines.getBusLines(kBus->getNo());
        for (vector <EnergyLine*>::iterator it = busLines.begin(); it != busLines.end(); ++it) {
            EnergyLine * line = *it;
            EnergyBus * from = line->getFrom();
            EnergyBus * to = line->getTo();
            double Bkm = line->getB();
            double Gkm = line->getG();
            double Dm, Vm;

            if (to == kBus)
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
        EnergyLine* line = lines[i];
        double Vk = line->getFrom()->getVoltage();
        double Vm = line->getTo()->getVoltage();
        double Dk = line->getFrom()->getAngle();
        double Dm = line->getTo()->getAngle();
        double Gkm = line->getG();

        ret += Gkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dk - Dm));
    }

    return 100 * ret;
}

void parseBusData()
{
    TiXmlDocument busDataFile("input/set_2/bus_data.xml");
    busDataFile.LoadFile();
    TiXmlElement * busElements = busDataFile.FirstChildElement("bus-data")->FirstChildElement("buses");
    TiXmlElement * busElement = busElements->FirstChildElement("bus");

    do
    {
        int no = atoi(busElement->FirstChildElement("no")->GetText());
        int code = atoi(busElement->FirstChildElement("code")->GetText());

        EnergyBus * bus = new EnergyBus(no, code);

        bus->setVoltage(
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("value")->GetText()),
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("min-value")->GetText()),
            atof(busElement->FirstChildElement("voltage")->FirstChildElement("max-value")->GetText())
        )->setAngle(
            atof(busElement->FirstChildElement("angle")->FirstChildElement("value")->GetText()),
            atof(busElement->FirstChildElement("angle")->FirstChildElement("min-value")->GetText()) * PI/180,
            atof(busElement->FirstChildElement("angle")->FirstChildElement("max-value")->GetText()) * PI/180
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
    TiXmlDocument lineDataFile("input/set_2/lines_data.xml");
    lineDataFile.LoadFile();
    TiXmlElement * linesElement = lineDataFile.FirstChildElement("lines-data")
                                                ->FirstChildElement("lines");
    TiXmlElement * lineElement = linesElement->FirstChildElement("line");

    do
    {
        int from = atoi(lineElement->FirstChildElement("bus-from")->GetText());
        int to   = atoi(lineElement->FirstChildElement("bus-to")->GetText());

        EnergyBus * busFrom = allBus.getBus(from);
        EnergyBus * busTo = allBus.getBus(to);

        double R = atof(lineElement->FirstChildElement("resistance")->FirstChildElement("active")->GetText());
        double X = atof(lineElement->FirstChildElement("resistance")->FirstChildElement("reactive")->GetText());

        EnergyLine * line = new EnergyLine(busFrom, busTo, R, X);
        lines.addLine(line);
    }
    while (lineElement = lineElement->NextSiblingElement("line"));

}

void parseGAparams()
{
    GASteadyStateGA::registerDefaultParameters(params);
    TiXmlDocument gaParamsFile("input/set_2/ga_params.xml");
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
