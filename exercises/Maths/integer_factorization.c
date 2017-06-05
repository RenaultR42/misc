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

void integerFactorization(size_t number)
{
        size_t i;
        size_t power;

        fprintf(stdout, "%zu = ", number);
        if (isPrimeNumber(number)) {
                fprintf(stdout, "%zu\n", number);
                return;
        }

        for (i = 2; i <= sqrt(number) + 1; i++) {
                if (isPrimeNumber(i)) {
                        if (number % i == 0) {
                                power = 0;
                                fprintf(stdout, "%zu", i);

                                while (number % i == 0) {
                                        number = number / i;
                                        power++;
                                }

                                /* More beautiful display */
                                if (power > 1)
                                        fprintf(stdout, "^%zu", power);

                                if (number >= 2) {
                                        fprintf(stdout, " * ");
                                } else { /* End of factorization */
                                        fprintf(stdout, "\n");
                                        return;
                                }
                        }
                }
        }

        fprintf(stdout, "%zu\n", number);
}

int main(int argc, char *argv[])
{
        size_t number;

        if (argc != 2) {
                fprintf(stderr, "integer_factorization [number]\n");
                return EXIT_FAILURE;
        }

        number = atoi(argv[1]);
        
        integerFactorization(number);
        return EXIT_SUCCESS;
}
