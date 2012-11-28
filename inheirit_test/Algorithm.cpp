#include "NewtonFunctions.cpp"
#include "Newton.cpp"
#include "Gauss.cpp"
#include <vector>

class Algorithm
{
private:
protected:
public:
    void solve()
    {
        Newton newton(10e-9, 10, false);
        vector <double> x;

        x.push_back(1);
        x.push_back(0);
        x.push_back(0);

        cout << newton.solve(x);
    };
};
