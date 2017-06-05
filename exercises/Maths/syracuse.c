#include <stdlib.h>
#include <stdio.h>

size_t syracuse(size_t number)
{
        size_t i = 0;

        while (number != 1) {
                i++;

                if (number % 2 == 0)
                        number = number / 2;
                else
                        number = 3 * number + 1;

                fprintf(stdout, "%zu\n", number);
        }

        return i;
}

int main(int argc, char *argv[])
{
        size_t number;

        if (argc != 2) {
                fprintf(stderr, "syracuses number\n");
                return EXIT_FAILURE;
        }

        number = atoi(argv[1]);

        fprintf(stdout, "Syracuse\'s conjecture reached in %zu loops\n", syracuse(number));
        return EXIT_SUCCESS;
}

