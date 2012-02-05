#include <string>
#include "Energy.h"

using namespace std;

int main(int argc, char** argv)
{
    // Вытаскиваем из параметров нужные
    char *s = NULL;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "s:")) != -1)
    {
        switch (c)
        {
            case 's':
              s = optarg;
              break;
            case '?':
              if (optopt == 's')
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

    Energy * energy = Energy::getInstance();
    energy->setup(inputDir, outputDir);
    energy->solve();

    return 0;
}
