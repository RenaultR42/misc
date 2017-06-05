#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int max(char **matrix, size_t size, char player, int n);
int min(char **matrix, size_t size, char player, int n);

const size_t DEFAULT_SIZE = 3;
const int N = 100;
const int MINVAL = 10000;
const int MAXVAL = -10000;

enum status {
        EMPTY = -1,
        NOT_FINISHED = 0,
        TIE = 1,
        X = 'X',
        O = 'O'
};

void cleanStdin(void) 
{ 
        int c; 

        do { 
                c = getchar(); 
        } while (c != '\n' && c != EOF); 
}

char isFinished(char **matrix, size_t size)
{
        size_t i, j;
        size_t cpt;
        char player;

        /* Line */
        for (i = 0; i < size; i++) {
                cpt = 0;
                player = matrix[0][i];
                for (j = 0; j < size - 1; j++) {
                        if (matrix[j][i] == matrix[j+1][i] && matrix[j][i] != EMPTY)
                                cpt++;
                }
                
                if (cpt == size - 1)
                        return player;
        }

        /* Column */
        for (i = 0; i < size; i++) {
                cpt = 0;
                player = matrix[i][0];
                for (j = 0; j < size - 1; j++) {
                        if (matrix[i][j] == matrix[i][j+1] && matrix[i][j] != EMPTY)
                                cpt++;
                }     
                
                if (cpt == size - 1)
                        return player;
        }

        /* Diagonal */   
        cpt = 0;
        player = matrix[0][0];
        for (i = 0; i < size - 1; i++) {
                if (matrix[i][i] == matrix[i+1][i+1] && matrix[i][i] != EMPTY)
                        cpt++;
        }

        if (cpt == size - 1)
                return player;
                
        /* Back-diagonal */
        cpt = 0; 
        player = matrix[0][size - 1];
        for (i = 0; i < size - 1; i++) {
                if (matrix[i][size-i-1] == matrix[i+1][size-i-2] && matrix[i][size-i-1] != EMPTY)
                        cpt++;
        }

        if (cpt == size - 1)
                return player;

        /* Check if board is full */
        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] == EMPTY)
                                return NOT_FINISHED;
                }
        }

        return TIE;
}

void playerChoice(char **matrix, size_t size, char player)
{
        int ret, y, x;

        do {
                fprintf(stdout, "Player %c, where do you want to play?\nLine: ", player);
                fflush(stdout);
                ret = scanf("%d", &y);

                fprintf(stdout, "Column: ");
                fflush(stdout);
                ret += scanf("%d", &x);

                cleanStdin();   

                if (ret != 2) {
                        fprintf(stderr, "You must choose 2 coordinates between 1 and %zu\n", size);
                        continue;
                }
        } while (x < 1 || x > size || y < 1 || y > size || matrix[y-1][x-1] != EMPTY);

        matrix[y-1][x-1] = player;
}

int eval(char **matrix, size_t size, char player)
{
        char winner;
        size_t i, j, cpt = 0;

        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] != EMPTY)
                                cpt++;
                }
        }


        if ((winner = isFinished(matrix, size)) != NOT_FINISHED) {
                if (winner == player)
                        return MINVAL - cpt;
                else if (winner != TIE)
                        return MAXVAL + cpt;
                else
                        return 0;
        }

        return 0;
}

int max(char **matrix, size_t size, char player, int n)
{
        int maxVal = MAXVAL, tmp;
        char otherPlayer = (player == X) ? O : X;
        size_t i, j;

        if (n == 0 || isFinished(matrix, size))
                return eval(matrix, size, player);

        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] == EMPTY) {
                                matrix[i][j] = otherPlayer;
                                tmp = min(matrix, size, player, n-1);

                                if (tmp > maxVal)
                                        maxVal = tmp;

                                matrix[i][j] = EMPTY;
                        }
                }
        }

        return maxVal;
}

int min(char **matrix, size_t size, char player, int n)
{
        int minVal = MINVAL, tmp;
        size_t i, j;

        if (n == 0 || isFinished(matrix, size))
                return eval(matrix, size, player);

        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] == EMPTY) {
                                matrix[i][j] = player;
                                tmp = max(matrix, size, player, n-1);

                                if (tmp < minVal)
                                        minVal = tmp;

                                matrix[i][j] = EMPTY;
                        }
                }
        }

        return minVal;
}

void iaChoice(char **matrix, size_t size, char player, int n)
{
        int i, j, tmp;
        int maxVal = MAXVAL;
        int maxi, maxj;

        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] == EMPTY) {
                                matrix[i][j] = player;

                                tmp = min(matrix, size, player, n-1);

                                if (tmp > maxVal) {
                                        maxVal = tmp;
                                        maxi = i;
                                        maxj = j;
                                }

                                matrix[i][j] = EMPTY;
                        }
                }
        }

        matrix[maxi][maxj] = player;
}

bool initMatrix(char **matrix, size_t size)
{
        size_t i;

        for (i = 0; i < size; i++) {
                matrix[i] = malloc(sizeof(size_t) * size);
                if (!matrix[i]) {
                        fprintf(stderr, "Can not allocate memory for row %zu of matrix\n", i);
                        for (; i >= 0; i--)
                                free(matrix[i]);
                        return false;
                } else {
                        memset(matrix[i], EMPTY, size);
                }
        }

        return true;
}

void freeMatrix(char **matrix, size_t size)
{
        size_t i;

        for (i = 0; i < size; i++)
                free(matrix[i]);
}

void displayMatrix(char **matrix, size_t size)
{
        size_t i, j;

        for (i = 0; i < size; i++) {
                for (j = 0; j < size; j++) {
                        if (matrix[i][j] != EMPTY)
                                fprintf(stdout, "%c\t", matrix[i][j]);
                        else
                                fprintf(stdout, ".\t");
                }

                fprintf(stdout, "\n");
        }

        fprintf(stdout, "\n\n");
}

void help(void)
{
        fprintf(stdout, "tic-tac-toe -i\n"
                        "tic-tac-toe -s size\n");
}

int main(int argc, char *argv[])
{
        bool ia = false, init;
        int opt;
        size_t size = DEFAULT_SIZE;
        char **matrix, player = X, finished = NOT_FINISHED;

        while ((opt = getopt(argc, argv, "ihs:")) != -1) {
                switch (opt) {
                case 's':
                   size = atoi(optarg);
                   break;
                case 'i':
                   ia = true;
                   srand(time(NULL));
                   break;
                case 'h':
                   help();
                   return EXIT_SUCCESS;
                   break;
                default:
                   help();
                   return EXIT_FAILURE;
                }
        }
        
        matrix = malloc(sizeof(size_t) * size);
        if (!matrix) {
                fprintf(stderr, "Can not allocate memory for matrix\n");
                return EXIT_FAILURE;
        }
        
        init = initMatrix(matrix, size);
        if (!init) {
                fprintf(stderr, "Can not allocate memory for matrix\n");
                return EXIT_FAILURE;
        }

        if (ia) {
                player = (rand() % 2) ? X : O;
                size = DEFAULT_SIZE;
        }

        while (!finished) {
                if (player == X)
                        player = O;
                else
                        player = X;

                displayMatrix(matrix, size);

                if ((ia && player == X) || !ia)
                        playerChoice(matrix, size, player);
                else if (ia)
                        iaChoice(matrix, size, player, N);

                finished = isFinished(matrix, size);
        }

        displayMatrix(matrix, size);
        if (finished == player)
                fprintf(stdout, "Player %c wins this game!\n", player);
        else
                fprintf(stdout, "Game over, no winner\n");

        freeMatrix(matrix, size);
        free(matrix);
        return EXIT_SUCCESS;
}
