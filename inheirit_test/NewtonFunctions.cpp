#include "NewtonFunctions.h"

double NewtonFunctions::getYCell(int i, int j, vector <double> x)
{
    double ret = 0;

    if (i == 0)
    {
        ret = 2 * x[j];
    }
    else if (i == 1)
    {
        if (j == 0)
        {
            ret = 4 * x[j];
        }
        else if (j == 1)
        {
            ret = 2 * x[j];
        }
        else
        {
            ret = -4;
        }
    }
    else if (i == 2)
    {
        if (j == 0)
        {
            ret = 6 * x[j];
        }
        else if (j == 1)
        {
            ret = -4;
        }
        else
        {
            ret = 2 * x[j];
        }
    }

    return ret;
}

double NewtonFunctions::getFCell(int i, vector <double> x)
{
    double ret = 0;

    if (i == 0)
    {
        ret = x[0] * x[0] + x[1] * x[1] + x[2] * x[2] - 1;
    }
    else if (i == 1)
    {
        ret = 2 * x[0] * x[0] + x[1] * x[1] - 4 * x[2];
    }
    else if (i == 2)
    {
        ret = 3 * x[0] * x[0] - 4 * x[1] + x[2] * x[2];
    }

    return ret;
}
