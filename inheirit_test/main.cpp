#include "NewtonFunctions.cpp"
#include "Gauss.cpp"
#include "Newton.cpp"

using namespace std;

int main() {
    Newton newton(10e-9, 10, false);
    vector <double> x;

    x.push_back(1);
    x.push_back(0);
    x.push_back(0);

    cout << newton.solve(x) << " " << x[0] << " " << x[1] << " " << x[2];

    return 0;
};
