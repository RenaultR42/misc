#include <stdlib.h>
#include <stdio.h>

void invert_digits(size_t number)
{
        fprintf(stdout, "%d", number % 10);

        do {
                fprintf(stdout, "%d", (number / 10) % 10);
                number /= 10;
        } while (number / 10 != 0);
}

int main(int argc, char *argv[])
{
        size_t number;

        if (argc != 2) {
                fprintf(stderr, "invert_digits number\n");
                return EXIT_FAILURE;
        }

        number = atoi(argv[1]);
        invert_digits(number);

        fprintf(stdout, "\n");
        return EXIT_SUCCESS;
}
