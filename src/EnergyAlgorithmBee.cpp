#include <iostream>
#include "EnergyAlgorithmBee.h"
#include "Bee.h"

using namespace std;

EnergyAlgorithmBee::EnergyAlgorithmBee(string input, string output):
        EnergyAlgorithm(input, output)
{
    count_of_scout_bees = 20;
    count_of_best_areas = 2;
    count_of_perspective_areas = 5;
    count_of_bees_to_best_area = 10;
    count_of_bees_to_perspective_area = 2;
    neighbourhood_value = 0.5;
    max_iterations = 50;

    count_of_bees = count_of_scout_bees +
            count_of_bees_to_best_area * count_of_best_areas +
            count_of_bees_to_perspective_area * count_of_perspective_areas;

    vector <pair <double, double> > limits;
    for (int i = 0, size = genBus.size(); i < size; ++i)
    {
        EnergyBus * bus = genBus[i];
        if (bus->isBalancedBus())
        {
         continue;
        }
        double * powerLimits = bus->getPowerGenLimits();
        limits.push_back(make_pair(powerLimits[0], powerLimits[1]));
     }

    Bee::limits = limits;
    Bee::dimension = limits.size();
    Bee::neighbourhood_value = neighbourhood_value;
}

void display_bees(vector <Bee> bees)
{
    for (int i = 0, size = bees.size(); i < size; ++i)
    {
        vector <double> cords = bees[i].get_cords();

        cout << i << ")" << bees[i].get_value() << ";" << 1 / bees[i].get_value() << ";";

        for (int j = 0, size_j = cords.size(); j < size_j; ++j)
        {
            cout << " " << cords[j];
        }

        cout << endl;
    }
}

void EnergyAlgorithmBee::solve()
{
    start_solve();

    vector <Bee> bees;

    for (int i = 0; i < count_of_bees; ++i)
    {
        Bee bee(this);
        bee.fly_to_random();
        bees.push_back(bee);
    }

    sort(bees.begin(), bees.end(), Bee::compare);

//    display_bees(bees);

    for (int i = 0; i < max_iterations; ++i)
    {
        progress(i, max_iterations);

        int j = 0, size, l = count_of_best_areas + count_of_perspective_areas;

        for (size = count_of_best_areas; j < size; ++j)
        {
            for (int k = 0; k < count_of_bees_to_best_area - 1; ++k)
            {
                bees[l++].fly_to_neighbor(bees[j]);
            }
        }

        for (size += count_of_perspective_areas; j < size; ++j)
        {
            for (int k = 0; k < count_of_bees_to_perspective_area - 1; ++k)
            {
                bees[l++].fly_to_neighbor(bees[j]);
            }
        }

        for (;l < count_of_bees;)
        {
            bees[l++].fly_to_random();
        }

        sort(bees.begin(), bees.end(), Bee::compare);

//        display_bees(bees);
    }

    display_bees(bees);

    fill(bees[0].get_cords());
    stop_solve();
}
