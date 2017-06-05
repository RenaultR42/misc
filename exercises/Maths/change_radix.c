#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void changeRadix(size_t number, size_t radix)
{
        size_t k, i, q;

        fprintf(stdout, "%zu = ", number);
        k = number / radix;

        while (number != 0 && number >= 0) {
                i = pow(radix, k); /* i = radix ^ k */
                q = number / i;    /* Quotient max */
                number -= (q * i);
                
                if (q != 0 && q >=0)
                        fprintf(stdout, "%zu * %zu^%zu", q, radix, k);

                if (number != 0 && number >= 0 && q != 0 && q >=0)
                        fprintf(stdout, " + ");

                k--;
        }

        fprintf(stdout, "\n");
}

int main(int argc, char *argv[])
{
        size_t number, radix;

        if (argc != 3) {
                fprintf(stderr, "radix number radix\n");
                return EXIT_FAILURE;
        }

        number = atoi(argv[1]);
        radix = atoi(argv[2]);
        
        if (radix < 2) {
                fprintf(stderr, "radix should be greather than 1\n");
                return EXIT_FAILURE;
        }

        changeRadix(number, radix);
        return EXIT_SUCCESS;
}
