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
    double (*getYCell)(int, int, vector <double>);
    vector <double> getF();
    double (*getFCell)(int, vector <double>);
    double getEPS();
public:
    Newton(
            double (*)(int, vector <double>),
            double (*)(int, int, vector <double>),
            double,
            int,
            bool
    );
    int solve (vector <double> &);
};

#endif
