#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>

bool isRightAngled(double hypotenuse, double side1, double side2)
{
        return ((hypotenuse * hypotenuse) == (side1 * side1 + side2 * side2));
}

double computeOtherSide(double hypotenuse, double side)
{
        return sqrt(hypotenuse * hypotenuse - side * side);
}

double computeHypotenuse(double side1, double side2)
{
        return sqrt(side1 * side1 + side2 * side2);
}

void help(void)
{
        fprintf(stdout, "pythagore -c hypotenuse lenght1 lenght2\n"
                        "pythagore -h lenght1 length2\n"
                        "pythagore -s hypotenuse lenght\n");
}

int main(int argc, char *argv[])
{
        double hypotenuse, side1, side2;
        int opt;

        while ((opt = getopt(argc, argv, "chs")) != -1) {
                switch (opt) {
                case 's':
                   if (argc == 4) {
                           hypotenuse = atof(argv[2]);
                           side1 = atof(argv[3]);
                           fprintf(stdout, "The lenght of the other side is %lf\n", computeOtherSide(hypotenuse, side1));
                   } else {
                        help();
                        return EXIT_FAILURE;
                   }
                   break;
                case 'c':
                   if (argc == 5) {
                           hypotenuse = atof(argv[2]);
                           side1 = atof(argv[3]);
                           side2 = atof(argv[4]);
                           if (isRightAngled(hypotenuse, side1, side2))
                                fprintf(stdout, "It\'s a right-angled triangle\n");
                           else
                                fprintf(stdout, "It is not a right-angled triangle\n");
                   } else {
                        help();
                        return EXIT_FAILURE;
                   }
                   break;
                case 'h':
                   if (argc == 4) {
                        side1 = atof(argv[2]);
                        side2 = atof(argv[3]);
                           fprintf(stdout, "The lenght of the hypotenuse side is %lf\n", computeHypotenuse(side1, side2));
                   } else {
                        help();
                        return EXIT_FAILURE;
                   }
                   break;
                default:
                   help();
                   return EXIT_FAILURE;
                }
        }

        return EXIT_SUCCESS;
}
