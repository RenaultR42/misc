#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

bool isPrimeNumber(size_t number)
{
        size_t i;
        size_t max = sqrt(number) + 1;

        if (number == 1 || number == 2)
                return true;

        for (i = 2; i <= max; i++) {
                if (number % i == 0)
                        return false;
        }

        return true;
}

int main(int argc, char *argv[])
{
        size_t max, i;
        
        if (argc != 2) {
                fprintf(stderr, "prime_numbers [maximum_value]\n");
                return EXIT_FAILURE;
        }

        max = atoi(argv[1]);

        for (i = 1; i <= max; i++) {
                if (isPrimeNumber(i))
                        fprintf(stdout, "%zu\n", i);
        }

        return EXIT_SUCCESS;
}
