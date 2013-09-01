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

    isWithRegulation = false;
    regulation = 0;
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
    double Dkm = Dk - Dm + (isWithRegulation ? (from->getNo() >= to->getNo() ? 1 : -1) * regulation : 0);
    double Gkm = getG();

    // @todo 100 вынести в констатну везде
    return -100 * Gkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dkm));
}

double EnergyLine::getReactivePower()
{
    double Vk = from->getVoltage();
    double Vm = to->getVoltage();
    double Dk = from->getAngle();
    double Dm = to->getAngle();
    double Dkm = Dk - Dm + (isWithRegulation ? regulation : 0);
    double Bkm = getB();

    return 100 * Bkm * (Vk * Vk + Vm * Vm - 2 * Vk * Vm * cos(Dkm));
}

EnergyBus * EnergyLine::getFrom()
{
    return from;
}

EnergyBus * EnergyLine::getTo()
{
    return to;
}

EnergyLine * EnergyLine::enableRegulation()
{
    isWithRegulation = true;
    return this;
}

EnergyLine * EnergyLine::disableRegulation()
{
    isWithRegulation = false;
    return this;
}


bool EnergyLine::isRegulationEnabled()
{
    return isWithRegulation;
}

double EnergyLine::getRegulation()
{
    return regulation;
}


EnergyLine * EnergyLine::setRegulation(double value)
{
    regulation = value;
    return this;
}

void EnergyLine::display()
{
    cout << endl << "*** Линия из " << from->getNo() << " в " << to->getNo() << " *** "
            << "G=" << getG() << "; B=" << getB() << "; R=" << R << "; X=" << X
            << "; P=" << getActivePower() << "; Q=" << getReactivePower();

    if (isWithRegulation) {
        cout << "; Regulation=" << regulation;
    }
}
