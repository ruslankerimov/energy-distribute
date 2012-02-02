#include <iostream>
#include "Energy.h"

using namespace std;

int main(int argc, char** argv)
{
    Energy * energy = Energy::getInstance();
    energy->setup();
    energy->solve();
    return 0;
}
