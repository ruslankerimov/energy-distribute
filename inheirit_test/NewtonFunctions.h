#ifndef NEWTON_FUNCTIONS_H_INCLUDED
#define NEWTON_FUNCTIONS_H_INCLUDED

#include <vector>

using namespace std;

class NewtonFunctions
{
protected:
    double getYCell(int, int, vector <double>);
    double getFCell(int, vector <double>);
};

#endif
