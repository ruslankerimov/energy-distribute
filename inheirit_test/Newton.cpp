#include <iostream>
#include "Newton.h"

template<typename T> T abs(T x) { return x < 0 ? -x : x; }

Newton::Newton(double eps, int max, bool debug)
{
    EPS = eps;
    MAX_STEPS = max;
    DEBUG = debug;
}

int Newton::solve(vector <double> &x)
{
    X = x;
    N = X.size();

    bool check = true;
    int ret = 0;
    int step = 0;

    while(check)
    {
        // @todo убрать присваивание
        F = getF();
        Y = getY();

        if ((ret = gauss(Y, F, Z)) != 1)
        {
            check = false;
        }

        if (DEBUG)
        {
            std::cout << endl << "************" << endl << "Шаг №" << step;
            std::cout << endl << "Y:";
            for (int j = 0; j < N; ++j)
            {
              std::cout << endl << j << ": ";
              for (int k = 0; k < N; ++k)
              {
                  std::cout << Y[j][k] << " ";
              }
            }
            std::cout << endl << "F:";
            for (int j = 0; j < N; ++j)
            {
              std::cout << endl << j << ": " << F[j];
            }
            std::cout << endl << "Решение:" << ret << endl;
            for (int j = 0; j < N; ++j)
            {
              std::cout << "x" << j << " = " << X[j] << "; ";
            }
            std::cout << endl << "Решение линейной системы: " << endl;
            for (int j = 0; j < N; ++j)
            {
              std::cout << "z" << j << " = " << Z[j] << "; ";
            }
            std::cout << endl << "Погрешность: " << getEPS();
            std::cout << endl << "************" << endl;
        }

        if (getEPS() < EPS)
        {
            check = false;
            ret = 1;
        }
        else if (++step > MAX_STEPS)
        {
            check = false;
            ret = 0;
        }
        else
        {
            for (int j = 0; j < N; ++j)
            {
                X[j] -= Z[j];
            }
        }
    }

    x = X;
    return ret;
}


vector <double> Newton::getF()
{
    vector <double> ret;

    for (int i = 0; i < N; ++i)
    {
        ret.push_back(getFCell(i, X));
    }

    return ret;
}

vector < vector <double> > Newton::getY()
{
    vector < vector <double> > ret;

    for (int i = 0; i < N; ++i)
    {
        vector <double> row;

        for (int j = 0; j < N; ++j)
        {
            row.push_back(getYCell(i, j, X));
        }
        ret.push_back(row);
    }

    return ret;
}

double Newton::getEPS()
{
    double ret = abs(Z[0]);

    // @todo iterator
    for (int i = 1; i < N; ++i)
    {
        if (abs(Z[i]) > ret)
        {
            ret = abs(Z[i]);
        }
    }

    return ret;
}
