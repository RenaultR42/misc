#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

void displayArray(size_t *array, size_t size)
{
        size_t i;

        for (i = 0; i < size; i++)
                printf("%d ", array[i]);

        printf("\n");
}

void generateArray(size_t *array, size_t size, size_t min, size_t max)
{
        size_t i;

        for (i = 0; i < size; i++)
                array[i] = rand() % (max - min) + min;
}

bool isSorted(size_t *array, size_t size)
{
        size_t i;

        for (i = 0; i < size - 1; i++) {
                if (array[i] > array[i + 1])
                        return false;
        }

        return true;
}

void mergeArrays(size_t *arrayA, size_t *arrayB, size_t sizeA, size_t sizeB)
{
        size_t a = 0, b = 0, i;
        const size_t mergedSize = sizeA + sizeB; 

        size_t *mergedArray = malloc(sizeof(size_t) * mergedSize);
        if (!mergedArray) {
                fprintf(stderr, "Can not allocate memory for merged array\n");
                exit(EXIT_FAILURE);
        }

        for (i = 0; i < mergedSize; i++) {
                if (b >= sizeB)  {	  
                        mergedArray[i] = arrayA[a];
                        a++;
                } else if (a >= sizeA) {
                        mergedArray[i] = arrayB[b];
                        b++;
                } else if (arrayA[a] < arrayB[b]) {
                        mergedArray[i] = arrayA[a];
                        a++;
                } else {
                        mergedArray[i] = arrayB[b];
                        b++;
                }
        }

        for (i = 0; i < mergedSize; i++)
                arrayA[i] = mergedArray[i];

        free(mergedArray);
}

void sort(size_t *array, size_t size)
{
        size_t p, q;

        for (p = 1; p < size; p *= 2) {
                for (q = 0; q < size - (size % (2 * p)); q += 2 * p)
                        mergeArrays(array + q, array + q + p, p, p);

                if (size % (2 * p) > p)
                        mergeArrays(array + q, array + q + p, p, size % p);
                else if (size % (2 * p) == p)
                        mergeArrays(array + q, NULL, p, 0);
        }
}

int main(int argc, char *argv[])
{
        size_t *array;
        size_t min, max, size;

        if (argc != 4) {
                fprintf(stderr, "binary_sort array_size min_value max_value\n");
                return EXIT_FAILURE;
        }

        srand(time(NULL));
        size = atoi(argv[1]);
        min = atoi(argv[2]);
        max = atoi(argv[3]);

        array = malloc(sizeof(size_t) * size);
        if (!array) {
                fprintf(stderr, "Can note allocate memory for array\n");
                return EXIT_FAILURE;
        }

        if (min > max)
                generateArray(array, size, max, min);
        else
                generateArray(array, size, min, max);

        fprintf(stdout, "** Initial array **\n");
        displayArray(array, size);

        fprintf(stdout, "\n** Sorted array **\n");
        sort(array, size);
        displayArray(array, size);

        if (!isSorted(array, size)) {
                fprintf(stderr, "Array is not correctly sorted !\n");
                return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}
