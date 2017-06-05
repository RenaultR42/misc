#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

bool isParallel(double AB, double AC, double AD, double AE)
{
        return ((AB / AC) == (AD / AE));
}

double computeAE(double AB, double AC, double AD)
{
        return AC * AD / AB;
}

void help(void)
{
        fprintf(stdout, "thales -p AB AC AD AE\n"
                        "thales -s AB AC AD\n\n"
                        "Thales' theorem with the following equation: AB   AD\n"
                        "                                             __ = __\n"
                        "                                             AC   AE\n");
}

int main(int argc, char *argv[])
{
        double AB, AC, AD, AE;
        int opt;

        while ((opt = getopt(argc, argv, "phs")) != -1) {
                switch (opt) {
                case 'p':
                   if (argc == 6) {
                           AB = atof(argv[2]);
                           AC = atof(argv[3]);
                           AD = atof(argv[4]);
                           AE = atof(argv[5]);
                           if (isParallel(AB, AC, AD, AE))
                                fprintf(stdout, "(BD) and (CE) are parallels\n");
                           else
                                fprintf(stdout, "(BD) and (CE) are not parallels\n");
                   } else {
                        help();
                        return EXIT_FAILURE;
                   }
                   break;
                case 's':
                   if (argc == 5) {
                           AB = atof(argv[2]);
                           AC = atof(argv[3]);
                           AD = atof(argv[4]);
                           fprintf(stdout, "The lenght of [AE] is %lf\n", computeAE(AB, AC, AD));
                   } else {
                        help();
                        return EXIT_FAILURE;
                   }
                   break;
                case 'h':
                   help();
                   return EXIT_SUCCESS;
                   break;
                default:
                   help();
                   return EXIT_FAILURE;
                }
        }

        return EXIT_SUCCESS;
}
