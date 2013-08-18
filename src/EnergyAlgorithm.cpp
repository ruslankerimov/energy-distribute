#include <iostream>
#include "EnergyAlgorithm.h"

using namespace std;

const long double PI = 2 * asin(1);

EnergyAlgorithm::EnergyAlgorithm(string input, string output):
        Newton(10e-3, 10, false)
{
    inputDir = input;
    outputDir = output;
    parseBusData();
    parseLinesData();
}

void EnergyAlgorithm::solve()
{

}

void EnergyAlgorithm::start_solve()
{
    progress(0, 1);
    t1 = clock();
}

void EnergyAlgorithm::stop_solve()
{
    t2 = clock();
    progress(1, 1);
    calculate();
    display();
    report();
}

void EnergyAlgorithm::fill(vector <double> cords)
{
    for (int i = 0, size = genWithoutBalanceBus.size(); i < size; ++i)
    {
        EnergyBus* bus = genWithoutBalanceBus[i];
        bus->setActivePowerGen(cords[i]);
    }
}

void EnergyAlgorithm::fill1(vector <double> cords)
{
    int j = 0;

    for (int i = 0, size = notGenBus.size(); i < size; ++i)
    {
        EnergyBus * bus = notGenBus[i];
        if (bus->isWithCompensation) {
            bus->setCompensation(cords[j++]);
        }
    }
}

double EnergyAlgorithm::fitness(vector <double> cords)
{
    fill(cords);
    calculate();
    double cost = allBus.cost();

    // @todo, можно от этого шага избивиться
    if ( ! allBus.checkRestractions())
    {
        cost += 100000;
    }

    return 1 / cost;
}

double EnergyAlgorithm::fitness1(vector <double> cords)
{
    fill1(cords);
    calculate();
    double cost = allBus.cost();

    // @todo, можно от этого шага избивиться
    if ( ! allBus.checkRestractions())
    {
        cost += 100000;
    }

    return 1 / cost;
}

void EnergyAlgorithm::calculate()
{
    vector <double> x;
    int n = allBus.size();

    // начальные значения
    for (int i = 0; i < n * 2; ++i) {
        x.push_back(i < n ? 0 : 1);
    }

    int solve = 0;

    solve = Newton::solve(x);

    if (solve > 0)
    {
        for (int i = 0; i < n; ++i) {
            EnergyBus* bus = allBus[i];

            bus->setAngle(x[i])->setVoltage(x[n + i]);
        }

        // Расчёт реактивной мощности
        for (int i = 0, size = genWithoutBalanceBus.size(); i < size; ++i)
        {
            EnergyBus* kBus = genWithoutBalanceBus[i];
            double ret = 0.0;
            double Vk = kBus->getVoltage();
            double Dk = kBus->getAngle();
            double Bkk = kBus->getB();
            vector <EnergyLine*> busLines = lines.getBusLines(kBus->getNo());

            ret += kBus->getReactivePowerLoad()/100 - Vk * Vk * Bkk;

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

                Dm = to->getAngle();
                Vm = to->getVoltage();

                ret -= Vk * Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
            }

            kBus->setReactivePowerGen(ret * 100);
        }
    }

    double activePower = allBus.getActivePowerLoad() + lines.getActivePower() - genWithoutBalanceBus.getActivePowerGen();
    double reactivePower = allBus.getReactivePowerLoad() + lines.getReactivePower() - genWithoutBalanceBus.getReactivePowerGen();
    activePower < 0 && (activePower = 0);
    balanceBus->setActivePowerGen(activePower)
            ->setReactivePowerGen(reactivePower);
}

double EnergyAlgorithm::getFCell(int i, vector <double> x)
{
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

    if (kBus->isBalancedBus())
    {
        if (isForP)
        {
            ret = Dk;
        }
        else
        {
            ret = Vk - kBus->getVoltage();
        }
    }
    else if (kBus->isGeneratorBus() && ! isForP)
    {
        ret = Vk - kBus->getVoltage();
    }
    else
    {
        if (isForP)
        {
            ret += Pk - Vk * Vk * Gkk;
        }
        else
        {
            ret += Qk + Vk * Vk * Bkk;
        }

        vector <EnergyLine*> busLines = lines.getBusLines(kBus->getNo());
        for (vector <EnergyLine*>::iterator it = busLines.begin(); it != busLines.end(); ++it)
        {
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

            if (isForP)
            {
                ret -= Vk * Vm * (Gkm * cos(Dk - Dm) + Bkm * sin(Dk - Dm));
            }
            else
            {
                ret += Vk * Vm * (Bkm * cos(Dk - Dm) - Gkm * sin(Dk - Dm));
            }
        }
    }

    return ret;
}

double EnergyAlgorithm::getYCell(int i, int j, vector <double> x)
{
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
        if (i == j)
        {
            ret = 1;
        }
        else
        {
            ret = 0;
        }

        return ret;
    }
    else if (kBus->isGeneratorBus() && ! isForP)
    {
        if (i == j)
        {
            ret = 1;
        }
        else
        {
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

void EnergyAlgorithm::display()
{
    cout << endl << "*** Исходные данные ***";
    cout << endl << "Генераторные узлы: ";
    for (int i = 0, size = genBus.size(); i
    < size; ++i)
    {
        cout << genBus[i]->getNo() << " ";
    }

    cout << endl << "Нагрузочные узлы: ";
    for (int i = 0, size = notGenBus.size(); i < size; ++i)
    {
        cout << notGenBus[i]->getNo() << " ";
    }

    cout << endl << "Балансирующий узел: " << balanceBus->getNo();

    cout << endl << endl << "*** Результаты ***";

    cout << endl << "Линии и их характеристики: ";
    for (int i = 0, size = lines.size(); i < size; ++i)
    {
        lines[i]->display();
    }

    cout << endl << "Узлы и их характеристики: ";
    for (int i = 0, size = allBus.size(); i < size; ++i)
    {
        allBus[i]->display();
    }

    cout << endl << "Нужная активная мощность: " << allBus.getActivePowerLoad() << " МВт";
    cout << endl << "Нужная реактивная мощность: " << allBus.getReactivePowerLoad() << " МВт";
    cout << endl << "Получившаяся активная мощность: " << allBus.getActivePowerGen() << " МВт";
    cout << endl << "Получившаяся реактивная мощность: " << allBus.getReactivePowerGen() << " МВт";
    cout << endl << "Получившаяся стоимость: " << allBus.cost();
    cout << endl << "Проверка по ограничениям: " << allBus.checkRestractions();

    cout << endl << "Мощность в линиях: активная: " << lines.getActivePower()
            << " реактивная: " << lines.getReactivePower();

    cout << endl << "Время расчёта: " << (double)(t2 - t1) / CLOCKS_PER_SEC << " c";
    cout << endl;
}

void EnergyAlgorithm::parseBusData()
{
    TiXmlDocument busDataFile(inputDir + "bus_data.xml");
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
                costPartElement = costPartElement->NextSiblingElement("part");
            }
            while (costPartElement);
            genBus.addBus(bus);
            if ( ! bus->isBalancedBus())
            {
                genWithoutBalanceBus.addBus(bus);
            }
        }
        else
        {
            if (busElement->FirstChildElement("compensation")) {
                bus->isWithCompensation = true;

                bus->setCompensationLimits(
                    atof(busElement->FirstChildElement("compensation")->FirstChildElement("min-value")->GetText()),
                    atof(busElement->FirstChildElement("compensation")->FirstChildElement("max-value")->GetText())
                );
            }
            notGenBus.addBus(bus);
        }

        if (bus->isBalancedBus())
        {
            balanceBus = bus;
        }

        allBus.addBus(bus);
        busElement = busElement->NextSiblingElement("bus");
    }
    while (busElement);
}

void EnergyAlgorithm::parseLinesData()
{
    TiXmlDocument lineDataFile(inputDir + "lines_data.xml");
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
        lineElement = lineElement->NextSiblingElement("line");
    }
    while (lineElement);
}

void EnergyAlgorithm::report()
{
    TiXmlDocument reportFile(outputDir + "report.xml");
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "utf-8", "");
    TiXmlElement * report = new TiXmlElement("report");

    TiXmlDocument busDataFile(inputDir + "bus_data.xml");
    busDataFile.LoadFile();
    TiXmlNode * busElements = busDataFile.FirstChildElement("bus-data")
            ->FirstChildElement("buses")->Clone();

    TiXmlDocument lineDataFile(inputDir + "lines_data.xml");
    lineDataFile.LoadFile();
    TiXmlNode * lineElements = lineDataFile.FirstChildElement("lines-data")
            ->FirstChildElement("lines")->Clone();

    TiXmlElement * solve = new TiXmlElement("solve");

    reportFile.LinkEndChild(decl);
    reportFile.LinkEndChild(report);
    report->LinkEndChild(busElements);
    report->LinkEndChild(lineElements);
    report->LinkEndChild(solve);

    TiXmlElement * busElement = busElements->FirstChildElement("bus");
    do
    {
        int no = atoi(busElement->FirstChildElement("no")->GetText());
        EnergyBus * bus = allBus.getBus(no);

        char str[20];
        sprintf(str, "%f", bus->getVoltage());
        busElement->FirstChildElement("voltage")->FirstChildElement("value")->Clear();
        busElement->FirstChildElement("voltage")->FirstChildElement("value")
                ->LinkEndChild(new TiXmlText(str));

        sprintf(str, "%f", bus->getAngle());
        busElement->FirstChildElement("angle")->FirstChildElement("value")->Clear();
        busElement->FirstChildElement("angle")->FirstChildElement("value")
                ->LinkEndChild(new TiXmlText(str));

        if (bus->isGeneratorBus())
        {
            sprintf(str, "%f", bus->getActivePowerGen());
            busElement->FirstChildElement("power-gen")->FirstChildElement("active")
                    ->FirstChildElement("value")->Clear();
            busElement->FirstChildElement("power-gen")->FirstChildElement("active")
                ->FirstChildElement("value")->LinkEndChild(new TiXmlText(str));

            sprintf(str, "%f", bus->getReactivePowerGen());
            busElement->FirstChildElement("power-gen")->FirstChildElement("reactive")
                    ->FirstChildElement("value")->Clear();
            busElement->FirstChildElement("power-gen")->FirstChildElement("reactive")
                ->FirstChildElement("value")->LinkEndChild(new TiXmlText(str));

            sprintf(str, "%f", bus->cost());
            busElement->FirstChildElement("cost")->LinkEndChild(new TiXmlElement("total"))
                    ->LinkEndChild(new TiXmlText(str));
        }

        sprintf(str, "%f", bus->getG());
        busElement->LinkEndChild(new TiXmlElement("G"))
                ->LinkEndChild(new TiXmlText(str));

        sprintf(str, "%f", bus->getB());
        busElement->LinkEndChild(new TiXmlElement("B"))
                ->LinkEndChild(new TiXmlText(str));

        busElement = busElement->NextSiblingElement("bus");
    }
    while (busElement);

    TiXmlElement * lineElement = lineElements->FirstChildElement("line");
    do
    {
        int from = atoi(lineElement->FirstChildElement("bus-from")->GetText());
        int to = atoi(lineElement->FirstChildElement("bus-to")->GetText());
        EnergyLine * line = lines.getLine(from, to);

        char str[20];
        sprintf(str, "%f", line->getG());
        lineElement->LinkEndChild(new TiXmlElement("G"))
                ->LinkEndChild(new TiXmlText(str));

        sprintf(str, "%f", line->getB());
        lineElement->LinkEndChild(new TiXmlElement("B"))
                ->LinkEndChild(new TiXmlText(str));

        lineElement = lineElement->NextSiblingElement("line");
    }
    while (lineElement);


    reportFile.SaveFile();
}

void EnergyAlgorithm::progress(int i, int max)
{
    float percent = i * 100 / max;
    static string bars = "Считаем ";

    if (bars.length() - 13 < percent)
    {
        bars += "|";
    }

    cout << bars << " " << percent << " %\r";
    fflush(stdout);
}
