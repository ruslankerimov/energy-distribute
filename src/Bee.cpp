#include "Bee.h"

int Bee::dimension;
double Bee::neighbourhood_value;
vector < pair <double, double> > Bee::limits;

Bee::Bee(EnergyAlgorithmBee * a)
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
    value = algorithm->fitness(new_cords);
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
