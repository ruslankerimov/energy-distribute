#include <stdlib.h>
#include <time.h>
#include <vector>
#include <utility>

#include <algorithm>
#include <iostream>

using namespace std;

class AlgorithmBee
{
private:
    void setup(int, int, int, int, int, double);

    int count_of_scout_bees;
    int count_of_best_areas;
    int count_of_perspective_areas;
    int count_of_bees_to_best_area;
    int count_of_bees_to_perspective_area;
    int count_of_bees;

    double neighbourhood_value;

    const static int MAX_ITERATIONS;
public:
    AlgorithmBee();

    double fitness(vector <double>);
    void solve();
};

AlgorithmBee::AlgorithmBee()
{
    setup(100, 10, 7, 5, 3, 0.01);
}


class Bee
{
private:
    AlgorithmBee * algorithm;
    double value;
    vector <double> cords;

    static double random_double(double, double);
    static bool is_rand_inited;
public:
    Bee(AlgorithmBee *);
    double get_value();
    vector <double> get_cords();
    void fly_to_random();
    void fly_to_neighbor(Bee);

    static int dimension;
    static double neighbourhood_value;
    static vector < pair <double, double> > limits;
    static bool compare(Bee, Bee);
};

int Bee::dimension;
double Bee::neighbourhood_value;
vector < pair <double, double> > Bee::limits;

Bee::Bee(AlgorithmBee * a)
{
    algorithm = a;
//    value = NULL;
}

double Bee::get_value()
{
    return value;
}

vector <double> Bee::get_cords()
{
    return cords;
}

void Bee::fly_to_random()
{
    vector <double> new_cords;

    for (int i = 0; i < dimension; ++i)
    {
        double min = limits[i].first;
        double max = limits[i].second;

        new_cords.push_back(random_double(min, max));
    }

    cords = new_cords;
    value = algorithm->fitness(cords);
}

void Bee::fly_to_neighbor(Bee bee)
{
    vector <double> bee_cords = bee.get_cords();
    vector <double> new_cords;
    vector <double>::iterator it_cords;
    vector < pair <double, double> >::iterator it_limits;

    for (it_cords = bee_cords.begin(), it_limits = limits.begin();
            it_cords != bee_cords.end(); ++it_cords, ++it_limits)
    {
        double min = *it_cords - neighbourhood_value;
        double max = *it_cords + neighbourhood_value;

        if (min < (*it_limits).first)
        {
            min = (*it_limits).first;
        }

        if (max > (*it_limits).second)
        {
            max = (*it_limits).second;
        }

        new_cords.push_back(random_double(min, max));
    }

    cords = new_cords;
    value = algorithm->fitness(cords);
}

bool Bee::compare(Bee a, Bee b)
{
    return a.get_value() > b.get_value();
}

bool Bee::is_rand_inited = false;

double Bee::random_double(double min, double max)
{
    if ( ! is_rand_inited)
    {
        srand(time(NULL));
        is_rand_inited = true;
    }

    return (rand() / ((double) RAND_MAX + 1.0)) * (max - min) + min;
}


void AlgorithmBee::setup(int c_of_scout_bees,
        int c_of_best_areas,
        int c_of_perspective_areas,
        int c_of_bees_to_best_area,
        int c_of_bees_to_perspective_area,
        double n_value)
{
    count_of_scout_bees = c_of_scout_bees;
    count_of_best_areas = c_of_best_areas;
    count_of_perspective_areas = c_of_perspective_areas;
    count_of_bees_to_best_area = c_of_bees_to_best_area;
    count_of_bees_to_perspective_area = c_of_bees_to_perspective_area;
    neighbourhood_value = n_value;

    count_of_bees = count_of_scout_bees +
            count_of_bees_to_best_area * count_of_best_areas +
            count_of_bees_to_perspective_area * count_of_perspective_areas;

    vector <pair <double, double> > limits;
    limits.push_back(make_pair(-10, 10));
    limits.push_back(make_pair(-10, 10));

    Bee::limits = limits;
    Bee::dimension = 2;
    Bee::neighbourhood_value = neighbourhood_value;
}


const int AlgorithmBee::MAX_ITERATIONS = 1000;

void AlgorithmBee::solve()
{
    vector <Bee> bees;

    for (int i = 0; i < count_of_bees; ++i)
    {
        Bee bee(this);
        bee.fly_to_random();
        bees.push_back(bee);
    }

    sort(bees.begin(), bees.end(), Bee::compare);

    for (int i = 0; i < MAX_ITERATIONS; ++i)
        {
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
        }

    for (int i = 0; i < count_of_bees; ++i)
    {
        vector <double> cords = bees[i].get_cords();

        cout << i << ")" << bees[i].get_value() << " " << cords[0] << " " << cords[1] << endl;
    }
}

double AlgorithmBee::fitness(vector <double> cords)
{
    double x = cords[0], y = cords[1];

    return x * x - y * y;
}

int main()
{
    AlgorithmBee algorithm;

    algorithm.solve();

    return 0;
}
