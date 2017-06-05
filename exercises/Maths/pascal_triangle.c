#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

size_t factorial(size_t number)
{
        size_t i, fact = 1;

        if (number == 0)
                return 1;

        for (i = 1; i <= number; i++)
                fact *= i;

        return fact;
}

size_t pascal(size_t n, size_t k)
{
        return factorial(n) / (factorial(k) * factorial(n - k));
}

void displayLine(size_t number)
{
        size_t i;

        for (i = 0; i <= number; i++)
                fprintf(stdout, "%zu\t", pascal(number, i));

        fprintf(stdout, "\n");
}

void displayTriangle(size_t nbLines)
{
        size_t i;

        for (i = 0; i <= nbLines; i++)
                displayLine(i);  
}

bool initMatrix(size_t **matrix, size_t nbLines)
{
        size_t i;

        for (i = 0; i < nbLines; i++) {
                matrix[i] = malloc(sizeof(size_t) * nbLines);
                if (!matrix[i]) {
                        fprintf(stderr, "Can not allocate memory for row %zu of matrix\n", i);
                        for (; i >=0; i--)
                                free(matrix[i]);
                        return false;
                } else {
                        memset(matrix[i], 0, nbLines);
                }
        }

        return true;
}

void freeMatrix(size_t **matrix, size_t nbLines)
{
        size_t i;

        for (i = 0; i < nbLines; i++)
                free(matrix[i]);
}

void fillMatrix(size_t **matrix, size_t nbLines)
{
        size_t i, j;

        matrix[0][0] = 1;

        for (i = 1; i < nbLines; i++) {
                matrix[i][0] = matrix[i-1][0];
                for (j = 1; j <= i; j++) {
                        matrix[i][j] = matrix[i-1][j-1] + matrix[i-1][j];
                }
        }
}

void displayMatrix(size_t **matrix, size_t nbLines)
{
        size_t i, j;

        for (i = 0; i < nbLines; i++) {
                for (j = 0; j < nbLines; j++) {
                        fprintf(stdout, "%zu\t", matrix[i][j]);
                }

                fprintf(stdout, "\n");
        }
}

void displayTriangleMatrix(size_t nbLines)
{
        size_t **matrix;
        bool init;

        matrix = malloc(sizeof(size_t) * nbLines);
        if (!matrix) {
                fprintf(stderr, "Can not allocate memory for matrix\n");
                return;
        }

        init = initMatrix(matrix, nbLines);
        if (init) {
                fillMatrix(matrix, nbLines);
                displayMatrix(matrix, nbLines);
                freeMatrix(matrix, nbLines);
        }

        free(matrix);
}

int main(int argc, char *argv[])
{
        size_t nbLines;

        if (argc != 2) {
                fprintf(stderr, "pascal_triangle number_lines\n");
                return EXIT_FAILURE;
        }

        nbLines = atoi(argv[1]);

        fprintf(stdout, "** Factorial compute method **\n\n");
        displayTriangle(nbLines);
        fprintf(stdout, "\n\n** Matrix compute method **\n\n");
        displayTriangleMatrix(nbLines);

        return EXIT_SUCCESS;
}
