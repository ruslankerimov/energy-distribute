#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

int COUNT_OF_SCOUT_BEES = 100;
int COUNT_OF_BEST_AREAS = 10;
int COUNT_OF_OTHER_AREAS = 5;
int COUNT_OF_BEES_TO_BEST_AREA = 10;
int COUNT_OF_BEES_TO_OTHER_AREA = 5;

double NEIGHBOURHOOD_VALUE = 0.01;

int MAX_ITERATIONS = 10000;

struct Bee
{
    vector <double> position;
    double value;
};

double random_double(double, double);
double random_float(float, float);
double fitness(Bee);
bool bee_comp(Bee, Bee);

int main()
{
    // Random generator init
    srand(time(NULL));

    // Bees
    vector <Bee> scouts_bees;
    vector <Bee> bees;
    Bee best_bee;

    // Limits bee positions
    vector < pair <double, double> > limits;

    limits.push_back(make_pair(-10, 10));
    limits.push_back(make_pair(-10, 10));

    // Creating scout bees
    for (int i = 0; i < COUNT_OF_SCOUT_BEES; ++i)
    {
        Bee bee;

        // Generate random positions
        for (int j = 0, size = limits.size(); j < size; ++j)
        {
            bee.position.push_back(random_double(limits[j].first, limits[j].second));
        }
        bee.value = fitness(bee);

        scouts_bees.push_back(bee);
    }

    sort(scouts_bees.begin(), scouts_bees.end(), bee_comp);

    bees = scouts_bees;
    best_bee = bees[0];

    for (int i = 0; i < MAX_ITERATIONS; ++i)
    {
        vector <Bee> new_bees;
        int j, size;

        for (j = 0, size = COUNT_OF_BEST_AREAS; j < size; ++j)
        {
            Bee etalon_bee = bees[j];
            new_bees.push_back(etalon_bee);

            for (int k = 1; k < COUNT_OF_BEES_TO_BEST_AREA; ++k)
            {
                Bee bee;

                for (int l = 0, sl = limits.size(); l < sl; ++l)
                {
                    double min = etalon_bee.position[l] - NEIGHBOURHOOD_VALUE;
                    double max = etalon_bee.position[l] + NEIGHBOURHOOD_VALUE;

                    if (max > limits[l].second)
                    {
                        max = limits[l].second;
                    }

                    if (min < limits[l].first)
                    {
                        min = limits[l].second;
                    }

                    bee.position.push_back(random_double(min, max));
                }

                bee.value = fitness(bee);
                new_bees.push_back(bee);
            }
        }

        for (size += COUNT_OF_OTHER_AREAS; j < size; ++j)
        {
            Bee etalon_bee = bees[j];
            new_bees.push_back(etalon_bee);

            for (int k = 1; k < COUNT_OF_BEES_TO_OTHER_AREA; ++k)
            {
                Bee bee;

                for (int l = 0, sl = limits.size(); l < sl; ++l)
                {
                    double min = etalon_bee.position[l] - NEIGHBOURHOOD_VALUE;
                    double max = etalon_bee.position[l] + NEIGHBOURHOOD_VALUE;

                    if (max > limits[l].second)
                    {
                        max = limits[l].second;
                    }

                    if (min < limits[l].first)
                    {
                        min = limits[l].second;
                    }

                    bee.position.push_back(random_double(min, max));
                }

                bee.value = fitness(bee);
                new_bees.push_back(bee);
            }
        }

        sort(new_bees.begin(), new_bees.end(), bee_comp);
//
//        for (int k = 0, size = new_bees.size(); k < size; ++k)
//        {
//            Bee bee = new_bees[k];
//
//            for (int j = 0, sj = bee.position.size(); j < sj; ++j)
//            {
//                cout << bee.position[j] << " ";
//            }
//            cout << "=" << bee.value << endl;
//        }

        best_bee = new_bees[0];

        cout << "Best: ";
        for (int j = 0, sj = best_bee.position.size(); j < sj; ++j)
        {
            cout << best_bee.position[j] << " ";
        }
        cout << "=" << best_bee.value << endl;

        bees = new_bees;
    }

//    for (int i = 0, size = scouts_bees.size(); i < size; ++i)
//    {
//        Bee bee = scouts_bees[i];
//
//        for (int j = 0, size2 = bee.position.size(); j < size2; ++j)
//        {
//            cout << bee.position[j] << " ";
//        }
//
//        cout << bee.value << endl;
//    }

    return 0;
}

double fitness(Bee bee)
{
    double x = bee.position[0];
    double y = bee.position[1];

    return -(1000 * (x - y * y) * (x - y * y) + (1 - x) * (1 - x));
}


double random_double(double min, double max)
{
    return (rand() / ((double) RAND_MAX + 1.0)) * (max - min) + min;
}
