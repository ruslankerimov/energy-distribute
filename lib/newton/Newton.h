#ifndef NEWTON_INCLUDED
#define NEWTON_INCLUDED

#include <cmath>
#include <vector>
#include "Gauss.h"

using namespace std;

/**
 * Класс для решения нелинейных систем уравнений
 */
class Newton
{
private:
    double EPS;
    bool DEBUG;
    int MAX_STEPS, N;
    vector <double> X, Z, F;
    vector < vector <double> > Y;
    vector <vector <double> > getY();
    vector <double> getF();
    double getEPS();
//protected:
    virtual double getYCell(int, int, vector <double>);
    virtual double getFCell(int, vector <double>);
protected:
    Newton(double, int, bool);
public:
    int solve (vector <double> &);
};

#endif
