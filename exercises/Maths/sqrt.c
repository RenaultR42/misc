#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Heron's algorithm */
double mySqrt(double a, double x, double b, size_t n)
{
        double middle;

        while (a * a >= x)
                a += 1;

        while (b * b <= x)
                b += 1;

        if (a * a == x)
                return a;

        if (b * b == x)
                return b;

        middle = (a + b) / 2;

        if (middle * middle > x && n > 0)
                return mySqrt(a, x, middle, n - 1);
        else if (middle * middle < x && n > 0)
                return mySqrt(middle, x, b, n - 1);
        else
                return middle;
}

int main(int argc, char *argv[])
{
        double x, a;
        size_t n;

        if (argc != 3) {
                fprintf(stderr, "sqrt number precision\n");
                return EXIT_FAILURE;
        }

        x = atof(argv[1]);
        n = atoi(argv[2]);

        a = floor(x - 1);

        fprintf(stdout, "mySqrt(%f) = %f ; sqrt(%f) = %f\n", x, mySqrt(a, x, 0, n), x, sqrt(x));
        return EXIT_SUCCESS;
}
