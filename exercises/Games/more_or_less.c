#include <stdio.h>
#include <stdlib.h>
#include <time.h>

size_t max(size_t nb1, size_t nb2)
{
        return nb1 > nb2 ? nb1 : nb2;
}

size_t min(size_t nb1, size_t nb2)
{
        return nb1 < nb2 ? nb1 : nb2;
}

size_t generateNumber(size_t min, size_t max)
{
        return (rand() % (max - min + 1)) + min;
}

size_t moreOrLess(size_t minVal, size_t maxVal, size_t valueToFind)
{
        size_t nbTurn = 0, userValue;
        int ret;

        do {
                fprintf(stdout, "Choose a number between %zu and %zu: ", minVal, maxVal);
                fflush(stdout);

                ret = scanf("%zu", &userValue);
                if (ret != 1) {
                        fprintf(stderr, "You must enter a integer number!\n");
                        continue;
                }

                nbTurn++;

                if (userValue < valueToFind) {
                        fprintf(stdout, "It\'s more!\n");
                        minVal = max(userValue, minVal);
                } else if (userValue > valueToFind) {
                        fprintf(stdout, "It\'s less!\n");
                        maxVal = min(userValue, maxVal);
                }
        } while (valueToFind != userValue); 

        return nbTurn;
}

int main(int argc, char *argv[])
{
        size_t minVal, maxVal, valueToFind;
        size_t nbTurn;

        srand(time(NULL));

        if (argc != 3) {
                fprintf(stderr, "more_or_less min_value max_value\n");
                return EXIT_FAILURE;
        }

        minVal = min(atoi(argv[1]), atoi(argv[2]));
        maxVal = max(atoi(argv[1]), atoi(argv[2]));
        valueToFind = generateNumber(minVal, maxVal);

        if (maxVal - minVal < 2) {
                fprintf(stderr, "Your min and max value should be different\n");
                return EXIT_FAILURE;
        }

        nbTurn = moreOrLess(minVal, maxVal, valueToFind);
        fprintf(stdout, "You find the value %zu after %zu turns.\n", valueToFind, nbTurn);

        return EXIT_SUCCESS;
}
