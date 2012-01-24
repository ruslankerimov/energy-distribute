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
double getYCell(int, int, vector <double>);
double getFCell(int, vector <double>);
double calculation(GARealGenome);

Bus * balanceBus;
LineCollection lines;
BusCollection allBus;
BusCollection genBus;
BusCollection notGenBus;


GAParameterList params;

int main()
{
    parseBusData();
    parseLinesData();
    parseGAparams();

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
//                << ": Gkk=" << (*it).G << "; Bkk=" << (*it).B
                << "; Pload=" << bus->getActivePowerLoad() << "; Qload=" << bus->getReactivePowerLoad();
    }



//    n = nodes.size();
//    k = genNodes.size();
//
//    params.parse(argc, argv, gaFalse);
//    GARealAlleleSetArray alleles;
//    cout << endl << "Лимиты генераторных узлов: ";
//    for (vector <node *>::iterator it = genNodes.begin(); it != genNodes.end(); ++it) {
//        cout << endl << "узел №" << (*(*it)).no << " : " << (*(*it)).Pgenmin << " - " << (*(*it)).Pgenmax << " МВт";
//        alleles.add((*(*it)).Pgenmin, (*(*it)).Pgenmax, GAAllele::INCLUSIVE, GAAllele::INCLUSIVE);
//    }
//    cout << endl;
//
//    GARealGenome genome(alleles, objective);
//    genome.crossover(GARealGenome::OnePointCrossover);
//
//    GASteadyStateGA ga(genome);
//    ga.parameters(params);
//    GANoScaling scaling;
//    ga.scaling(scaling);
//    GATournamentSelector selector;
//    ga.selector(selector);
//    ga.evolve();
//
//    cout << endl << "*** Результаты ***";
//    cout << endl << "Нужная активная мощность: " << sumPload << " МВт";
//
//    genome = ga.statistics().bestIndividual();
////  cout << endl << ga.statistics() << ga.statistics().bestIndividual() << endl;
//    cout << endl << "Получившиеся мощности: ";
//    for (int i = 0, size = genome.length(); i < size; ++i) {
//        cout << endl << "узел №" << (*(genNodes[i])).no << " " << genome.gene(i) << " МВт";
//    }
//    double summary[3];
//    getSummary(genome, summary);
//    cout << endl << "Получившаяся суммарная мощность: " << summary[0] << " МВт";
//    cout << endl << "Получившаяся стоимость: " << summary[2];
//
//    cout << endl << "Расчёт системы: ";
//    calculation(genome);
//
//    cout << endl << endl;

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

        busTo->addLinkedBus(busFrom);
        busFrom->addLinkedBus(busTo);
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
