#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        float celsius, fahr;
        int max, min, step;

        max = 300;
        min = 0;
        step = 20;

        printf("Celsius\tFahrenheit\n");

        for (celsius = max; celsius >= min; celsius -= step) {
                fahr = (9 * celsius / 5) + 32;
                printf("%f\t%f\n", celsius, fahr);
        }

        return EXIT_SUCCESS;
}
