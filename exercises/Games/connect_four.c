#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int min(char **matrix, size_t nbLines, size_t nbCol, size_t col, char player, int n);
int max(char **matrix, size_t nbLines, size_t nbCol, size_t col, char player, int n);

const size_t DEFAULT_COL_SIZE = 7;
const size_t DEFAULT_LINES_SIZE = 6;
const size_t ALIGNED_WIN = 4;
const int N = 7;
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

bool validPoint(size_t x, size_t y)
{
        return ((x >= 0 && x < DEFAULT_COL_SIZE) && (y >= 0 && y < DEFAULT_LINES_SIZE));
}

size_t nbAdjacentToken(char **matrix, size_t x, size_t y, int moveH, int moveV, char player)
{
        size_t tmpX, tmpY;
        size_t nb = 1;

        tmpX = x + moveH;
        tmpY = y + moveV;

        while (validPoint(tmpX, tmpY)) {
                if (matrix[tmpY][tmpX] == player)
                        nb++;
                else
                        break;

                tmpX += moveH;
                tmpY += moveV;
        }

        return nb;
}

size_t lastInsert(char **matrix, size_t nbLines, size_t x)
{
        size_t i;

        for (i = 0; i < nbLines; i++) {
                if (matrix[i][x] == EMPTY)
                        return i - 1;
        }

        return 0;
}

size_t umax(size_t a, size_t b)
{
        return (a > b) ? a : b;
}

char isFinished(char **matrix, size_t nbLines, size_t nbCol, int x, char player)
{
        size_t maxVal, y = lastInsert(matrix, nbLines, x);
        size_t i, j;

        /* Vertical */
        maxVal = nbAdjacentToken(matrix, x, y, 0, -1, player);

        /* Horizontal */
        maxVal = umax(maxVal, nbAdjacentToken(matrix, x, y, 1, 0, player) + \
                nbAdjacentToken(matrix, x, y, -1, 0, player) - 1);

        /* Diagonal */
        maxVal = umax(maxVal, nbAdjacentToken(matrix, x, y, 1, 1, player) + \
                nbAdjacentToken(matrix, x, y, -1, -1, player) - 1);

        /* Back diagonal */
        maxVal = umax(maxVal, nbAdjacentToken(matrix, x, y, 1, -1, player) + \
                nbAdjacentToken(matrix, x, y, -1, 1, player) - 1);

        if (maxVal >= ALIGNED_WIN)
                return player;

        /* Check if board is full */
        for (i = 0; i < nbLines; i++) {
                for (j = 0; j < nbCol; j++) {
                        if (matrix[i][j] == EMPTY)
                                return NOT_FINISHED;
                }
        }

        return TIE;
}

int insert(char **matrix, size_t nbLines, size_t col, char player)
{
        size_t i;

        for (i = 0; i < nbLines; i++) {
                if (matrix[i][col] == EMPTY) {
                        matrix[i][col] = player;
                        return i + 1;
                }
        }

        return 0;
}

int playerChoice(char **matrix, size_t nbLines, size_t nbCol, char player)
{
        int ret, x;

        do {
                fprintf(stdout, "Player %c, where do you want to play?\nColumn: ", player);
                fflush(stdout);
                ret = scanf("%d", &x);

                cleanStdin();   

                if (ret != 1) {
                        fprintf(stderr, "You must choose column between 1 and %zu\n", nbCol);
                        continue;
                }
        } while (x < 1 || x > nbCol || !insert(matrix, nbLines, x - 1, player));

        return x;
}

int eval(char **matrix, size_t nbLines, size_t nbCol, size_t col, char player)
{
        char winner;
        size_t i, j, cpt = 0;

        for (i = 0; i < nbLines; i++) {
                for (j = 0; j < nbCol; j++) {
                        if (matrix[i][j] != EMPTY)
                                cpt++;
                }
        }


        if ((winner = isFinished(matrix, nbLines, nbCol, col, player)) != NOT_FINISHED) {
                if (winner == player)
                        return MINVAL - cpt;
                else if (winner != TIE)
                        return MAXVAL + cpt;
                else
                        return 0;
        }

        return 0;
}

int max(char **matrix, size_t nbLines, size_t nbCol, size_t col, char player, int n)
{
        int maxVal = MAXVAL, tmp, y;
        char otherPlayer = (player == X) ? O : X;
        size_t i;

        if (n == 0 || isFinished(matrix, nbLines, nbCol, col, player))
                return eval(matrix, nbLines, nbCol, col, player);

        for (i = 0; i < nbCol; i++) {
                if ((y = insert(matrix, nbLines, i, otherPlayer))) {
                        tmp = min(matrix, nbLines, nbCol, i, player, n-1);

                        if (tmp > maxVal)
                                maxVal = tmp;

                        matrix[y-1][i] = EMPTY;
                }
        }

        return maxVal;
}

int min(char **matrix, size_t nbLines, size_t nbCol, size_t col, char player, int n)
{
        int minVal = MINVAL, tmp, y;
        size_t i;

        if (n == 0 || isFinished(matrix, nbLines, nbCol, col, player))
                return eval(matrix, nbLines, nbCol, col, player);

        for (i = 0; i < nbCol; i++) {
                if ((y = insert(matrix, nbLines, i, player))) {
                        tmp = max(matrix, nbLines, nbCol, i, player, n-1);

                        if (tmp < minVal)
                                minVal = tmp;

                        matrix[y-1][i] = EMPTY;
                }
        }

        return minVal;
}

int iaChoice(char **matrix, size_t nbLines, size_t nbCol, char player, int n)
{
        int i, tmp, y;
        int maxVal = MAXVAL;
        int maxi;

        for (i = 0; i < nbCol; i++) {
                if ((y = insert(matrix, nbLines, i, player))) {
                        tmp = min(matrix, nbLines, nbCol, i, player, n-1);

                        if (tmp > maxVal) {
                                maxVal = tmp;
                                maxi = i;
                        }

                        matrix[y-1][i] = EMPTY;
                }
        }

        insert(matrix, nbLines, maxi, player);
        return maxi + 1;
}

void displayMatrix(char **matrix, size_t nbLines, size_t nbCol)
{
        int i, j;

        for (i = nbLines - 1; i >= 0; i--) {
                for (j = nbCol - 1; j >= 0; j--) {
                        if (matrix[i][j] != EMPTY)
                                fprintf(stdout, "%c ", matrix[i][j]);
                        else
                                fprintf(stdout, ". ");
                }

                fprintf(stdout, "\n");
        }

        fprintf(stdout, "\n");

        for (i = nbCol; i > 0; i--)
                fprintf(stdout, "%d ", i);
                

        fprintf(stdout, "\n\n");
}

bool initMatrix(char **matrix, size_t nbLines, size_t nbCol)
{
        size_t i;

        for (i = 0; i < nbLines; i++) {
                matrix[i] = malloc(sizeof(size_t) * nbCol);
                if (!matrix[i]) {
                        fprintf(stderr, "Can not allocate memory for row %zu of matrix\n", i);
                        for (; i >= 0; i--)
                                free(matrix[i]);
                        return false;
                } else {
                        memset(matrix[i], EMPTY, nbCol);
                }
        }

        return true;
}

void freeMatrix(char **matrix, size_t nbLines)
{
        size_t i;

        for (i = 0; i < nbLines; i++)
                free(matrix[i]);
}

int main(int argc, char *argv[])
{
        bool init, ia = false;
        int opt, x;
        size_t nbCol = DEFAULT_COL_SIZE, nbLines = DEFAULT_LINES_SIZE;
        char **matrix, player = X, finished = NOT_FINISHED;
        
        while ((opt = getopt(argc, argv, "ih")) != -1) {
                switch (opt) {
                case 'i':
                   ia = true;
                   srand(time(NULL));
                   break;
                case 'h':
                   fprintf(stdout, "connect_four [-i]\n");
                   return EXIT_SUCCESS;
                   break;
                default:
                   fprintf(stdout, "connect_four [-i]\n");
                   return EXIT_FAILURE;
                }
        }
        
        matrix = malloc(sizeof(size_t) * nbLines);
        if (!matrix) {
                fprintf(stderr, "Can not allocate memory for matrix\n");
                return EXIT_FAILURE;
        }
        
        init = initMatrix(matrix, nbLines, nbCol);
        if (!init) {
                fprintf(stderr, "Can not allocate memory for matrix\n");
                return EXIT_FAILURE;
        }
        
        if (ia)
                player = (rand() % 2) ? X : O;

        while (!finished) {
                if (player == X)
                        player = O;
                else
                        player = X;

                displayMatrix(matrix, nbLines, nbCol);

                if ((ia && player == X) || !ia)
                        x = playerChoice(matrix, nbLines, nbCol, player);
                else if (ia)
                        x = iaChoice(matrix, nbLines, nbCol, player, N);

                finished = isFinished(matrix, nbLines, nbCol, x - 1, player);
        }

        displayMatrix(matrix, nbLines, nbCol);
        if (finished == player)
                fprintf(stdout, "Player %c wins this game!\n", player);
        else
                fprintf(stdout, "Game over, no winner\n");

        freeMatrix(matrix, nbLines);
        free(matrix);
}
