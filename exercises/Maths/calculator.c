#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

size_t factorial(size_t number)
{
        size_t i, fact = 1;

        if (number == 0)
                return 1;

        for (i = 1; i <= number; i++)
                fact *= i;

        return fact;
}

size_t lcm(size_t n1, size_t n2)
{
        size_t i;
        size_t min = (n1 < n2) ? n1 : n2;

        for (i = 2; i <= min; ++i)
                if (n1 % i == 0 && n2 % i == 0)
                        return i;

        return 0;
}

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

int main(void)
{
        double a, b, ans;
        int ret;
        char opt;

        while (true) {
                fprintf(stdout, "> ");
                if ((ret = scanf("%lf %lf %c", &a, &b, &opt)) <= 1) {
                        scanf("%c", &opt);
                        b = a;
                        a = ans;
                }

                switch (opt) {
                case 'q':
                        return EXIT_SUCCESS;
                        break;
                case '+':
                        ans = a + b;
                        break;
                case '-':
                        ans = a - b;
                        break;
                case '/':
                        if (b != 0) {
                                ans = a / b;
                        } else {
                                fprintf(stderr, "We can not divide by zero\n");
                                continue;
                        }
                        break;
                case '*':
                        ans = a * b;
                        break;
                case '^':
                        ans = pow(a, b);
                        break;
                case '%':
                        ans = (size_t) a % (size_t) b;
                        break;
                case '!':
                        ans = factorial((ret == 0) ? a : b);
                        break;
                case 'g':
                        ans = gcd((size_t) a, (size_t) b);
                        break;
                case 'l':
                        ans = lcm((size_t) a, (size_t) b);
                        break; 
                default:
                        fprintf(stderr, "Unknow operation\n");
                        continue;
                        break;
                }

                fprintf(stdout, "%lf\n", ans);
        }

        return EXIT_SUCCESS;
}
