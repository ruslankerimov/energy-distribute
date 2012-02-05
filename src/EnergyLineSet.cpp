#include "EnergyLineSet.h"

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

double EnergyLineSet::getActivePower()
{
    double ret = 0.0;

    for (it = lines.begin(); it != lines.end(); ++it)
    {
        ret += it->second->getActivePower();
    }

    return ret;
}

double EnergyLineSet::getReactivePower()
{
    double ret = 0.0;

    for (it = lines.begin(); it != lines.end(); ++it)
    {
        ret += it->second->getReactivePower();
    }

    return ret;
}

vector <EnergyLine*> EnergyLineSet::getBusLines(int busNo)
{
    return links[busNo];
}

EnergyLine* EnergyLineSet::operator [](int n)
{
    it = lines.begin();
    // @todo
    for (int i = 0; i < n; ++i, ++it);
    return it->second;
}
