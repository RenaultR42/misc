#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        float celsius, fahr;
        int min, max, step;

        min = 0;
        max = 300;
        step = 20;

        celsius = min;
        printf("Celsius\tFahrenheit\n");

        while (celsius <= max) {
                fahr = (9 * celsius / 5) + 32;
                printf("%f\t%f\n", celsius, fahr);
                celsius += step;
        }

        return EXIT_SUCCESS;
}
