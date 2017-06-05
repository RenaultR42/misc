#include <stdio.h>
#include <stdlib.h>

/* Euclide's algorithm */
size_t gcd(size_t n1, size_t n2)
{
        while (n1 != n2) {
                if (n1 > n2)
                        n1 = n1 - n2;
                else
                        n2 = n2 - n1;
        }

        return n1;
}

int main(int argc, char *argv[])
{
        size_t n1, n2;

        if (argc != 3) {
                fprintf(stderr, "gcd n1 n2\n");
                return EXIT_FAILURE;
        }

        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);

       printf("GCD of %zu and %zu is %zu\n", n1, n2, gcd(n1, n2));
       return EXIT_SUCCESS;
}
