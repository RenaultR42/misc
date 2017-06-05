#include <stdlib.h>
#include <stdio.h>

size_t fibonacci(size_t n, size_t fn1, size_t fn)
{
        if (n <= 0) {
                return fn;
        } else {
                fprintf(stdout, "%zu\n", fn + fn1);
                return fibonacci(n - 1, fn, fn + fn1);
        }
}

int main(int argc, char *argv[])
{
        int n = 0;

        if (argc != 2) {
                fprintf(stderr, "fibonacci n\n"
                                "0 < n <= 46\n");
                return EXIT_FAILURE;
        }

        n = atoi(argv[1]);
        fibonacci(n, 1, 0);

        return EXIT_SUCCESS;
}
