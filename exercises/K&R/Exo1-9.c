#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        int c;

        while ((c = getchar()) != EOF) {
                if (c == ' ') {
                        while ((c = getchar()) == ' ')
                                ;

                        putchar(' ');
                }

                putchar(c);
        }

        return EXIT_SUCCESS;
}
