#ifndef BEE_H_INCLUDED
#define BEE_H_INCLUDED

#include <stdlib.h>
#include <time.h>
#include <vector>
#include <utility>

#include "EnergyAlgorithmBee.h"

using namespace std;

class Bee
{
private:
    EnergyAlgorithmBee * algorithm;
    double value;
    vector <double> cords;

    static double random_double(double, double);
    static bool is_rand_inited;
public:
    Bee(EnergyAlgorithmBee *);
    double get_value();
    vector <double> get_cords();
    void fly_to_random();
    void fly_to_neighbor(Bee);

    static int dimension;
    static double neighbourhood_value;
    static vector < pair <double, double> > limits;
    static bool compare(Bee, Bee);
};

#endif
