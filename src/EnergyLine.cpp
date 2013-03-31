#include "EnergyLine.h"

using namespace std;

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
    return -G;
}

double EnergyLine::getB()
{
    return B;
}

double EnergyLine::getActivePower()
{

    double Vk = from->getVoltage();
    double Vm = to->getVoltage();
    double Dk = from->getAngle();
    double Dm = to->getAngle();
    double Gkm = getG();

    // @todo 100 вынести в констатну везде
    return -100 * Gkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dk - Dm));
}

double EnergyLine::getReactivePower()
{
    double Vk = from->getVoltage();
    double Vm = to->getVoltage();
    double Dk = from->getAngle();
    double Dm = to->getAngle();
    double Bkm = getB();

    return 100 * Bkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dk - Dm));
}

EnergyBus * EnergyLine::getFrom()
{
    return from;
}

EnergyBus * EnergyLine::getTo()
{
    return to;
}

void EnergyLine::display()
{
    cout << endl << "*** Линия из " << from->getNo() << " в " << to->getNo() << " *** "
            << "G=" << getG() << "; B=" << getB() << "; R=" << R << "; X=" << X
            << "; P=" << getActivePower() << "; Q=" << getReactivePower();
}
