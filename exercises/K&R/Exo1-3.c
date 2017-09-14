#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        float celsius, fahr;
        int min, max, inc;

        min = 0;
        max = 300;
        step = 20;

        fahr = min;
        printf("Fahr\tCelsius\n");

        while (fahr <= max) {
                celsius = 5 * (fahr - 32) / 9;
                printf("%f\t%f\n", fahr, celsius);
                fahr += step;
        }

        return EXIT_SUCCESS;
}
