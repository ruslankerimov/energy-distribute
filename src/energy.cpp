#include <string>
#include <unistd.h>
#include "EnergyAlgorithmACO.h"
#include "EnergyAlgorithmABC.h"
#include "EnergyAlgorithmGA.h"

using namespace std;

int main(int argc, char** argv)
{
    // Вытаскиваем из параметров нужные
    char *s = NULL;
    char *t = NULL;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "s:t:")) != -1)
    {
        switch (c)
        {
            case 's':
              s = optarg;
              break;
            case 't':
              t = optarg;
              break;
            case '?':
              if (optopt == 's')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
              else if (optopt == 't')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
              else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
              else
                fprintf(
                            stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt
                        );
              return 1;
            default:
              abort();
        }
    }

    string inputDir = "input/set_";
    string outputDir = "output/set_";
    if (s)
    {
        inputDir += s;
        inputDir += "/";
        outputDir += s;
        outputDir += "/";
    }
    else
    {
        inputDir += "default/";
        outputDir += "default/";
    }

    string type = t;

    if (type == "ACO") {
        EnergyAlgorithmACO algorithm(inputDir, outputDir);
        algorithm.solve();
    } else if (type == "ABC") {
        EnergyAlgorithmABC algorithm(inputDir, outputDir);
        algorithm.solve();
    } else {
        EnergyAlgorithmGA algorithm(inputDir, outputDir);
        algorithm.solve();
    }

    return 0;
}
