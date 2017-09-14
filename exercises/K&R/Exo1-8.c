#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        int c;
        int nbnl = 0, nbs = 0, nbt = 0, nbc = 0;

        while ((c = getchar()) != EOF) {
                nbc++;

                if (c == '\n')
                        nbnl++;

                else if (c == '\t')
                        nbt++;

                else if (c == ' ')
                        nbs++;
        }

        printf("\n\nChar: %d\nTab: %d\nSpaces: %d\nNew lines: %d\n", nbc, nbt, nbs, nbnl);

        return EXIT_SUCCESS;
}
