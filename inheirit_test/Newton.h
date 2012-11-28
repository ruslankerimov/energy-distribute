#ifndef NEWTON_INCLUDED
#define NEWTON_INCLUDED

#include <cmath>
#include <vector>
#include "NewtonFunctions.h"
#include "Gauss.h"

using namespace std;

/**
 * Класс для решения нелинейных систем уравнений
 */
class Newton:
        public NewtonFunctions
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
public:
    Newton(
            double,
            int,
            bool
    );
    int solve (vector <double> &);
};

#endif
