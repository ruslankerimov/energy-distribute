#include <iostream>
#include "EnergyLine.h"

EnergyLine::EnergyLine(EnergyBus * fromBus, EnergyBus * toBus, double r, double x)
{
    from = fromBus;
    to   = toBus;
    R = r;
    X = x;
    G = R / (X * X + R * R);
    B = X / (X * X + R * R);
}

double EnergyLine::getR()
{
    return R;
}

double EnergyLine::getX()
{
    return X;
}

double EnergyLine::getG()
{
    return G;
}

double EnergyLine::getB()
{
    return B;
}

EnergyBus * EnergyLine::getFrom()
{
    return from;
}

EnergyBus * EnergyLine::getTo()
{
    return to;
}


EnergyLineSet::EnergyLineSet()
{

}

EnergyLineSet * EnergyLineSet::addLine(EnergyLine * line)
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

EnergyLine * EnergyLineSet::getLine(int from, int to)
{
    int key = from * 1000 + to;
    if (lines.count(key) < 1)
    {
        key = to * 1000 + from;
    }
    // @todo узкое место, нужна проверка на map::end()
    return lines.find(key)->second;
}

int EnergyLineSet::size()
{
    return lines.size();
}

vector <EnergyLine*> EnergyLineSet::getBusLines(int busNo)
{
    return links[busNo];
}

EnergyLine* EnergyLineSet::operator [](int n)
{
    it = lines.begin();
    for (int i = 0; i < n; ++i, ++it);
    return it->second;
}
