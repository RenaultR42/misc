#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NB_SCORE_MAX 11

int allScoring[][NB_SCORE_MAX] = { 
        { 9, 6, 4, 3, 2, 0 }, /* 1950 - 1959 */
        { 8, 6, 4, 3, 2, 1, 0 }, /* 1960 (all) - 1961 (TEAM only) */
        { 9, 6, 4, 3, 2, 1, 0 }, /* 1961 - 1990 */
        { 10, 6, 4, 3, 2, 1, 0 }, /* 1991 - 2002 */
        { 10, 8, 6, 5, 4, 3, 2, 1, 0 }, /* 2003 - 2009 */
        { 25, 18, 15, 12, 10, 8, 6, 4, 2, 1, 0 } /* 2010 - */
};

enum status {
        ERROR                   = -4,
        IMPOSSIBLE              = -3,
        NOT_CHAMPION            = -2,
        ALREADY_CHAMPION        = -1,
};

enum options {
        BEST_SEASON,
        COMPUTE_FINAL_CHAMPIONSHIP,
};

enum flags {
        DRIVER                  = 0x1,
        TEAM                    = 0x2,
        LAST_RACE_DOUBLE        = 0x4, /* Only for 2014 */
};

int computeEndOfSeason(unsigned int score, unsigned int nbGPToDo, unsigned int scoring[], int flags)
{
        if ((flags & DRIVER) && (flags & LAST_RACE_DOUBLE))
                return score + scoring[0] * (nbGPToDo - 1) + scoring[0] * 2; 
        else if (flags & DRIVER)
                return score + scoring[0] * nbGPToDo;
        else if ((flags & TEAM) && (flags & LAST_RACE_DOUBLE))
                return score + (scoring[0] + scoring[1]) * (nbGPToDo - 1) + (scoring[0] + scoring[1]) * 2; 
        else if (flags & TEAM)
                return score + (scoring[0] + scoring[1]) * nbGPToDo;
}

int incrementGP(unsigned int score, unsigned int nbGPToDo, unsigned int scoring[], int flags)
{
        int coef = 1;

        if ((flags & LAST_RACE_DOUBLE) && nbGPToDo == 0)
                coef = 2;
 
        if (flags & DRIVER)
                return score + scoring[0] * coef;
        else if (flags & TEAM)
                return score + (scoring[0] + scoring[1]) * coef;        
}

int worldChampion(unsigned int score1, unsigned int score2, unsigned int nbGPToDo,
                  unsigned int scoring[], int flags)
{
	int i, score2EndOfSeason;

	score2EndOfSeason = computeEndOfSeason(score2, nbGPToDo, scoring, flags);

	if (score1 >= score2EndOfSeason)
		return ALREADY_CHAMPION;

	for (i = 1; i <= nbGPToDo && score1 <= score2EndOfSeason; i++) {
	        score1 += incrementGP(score1, nbGPToDo - i, scoring, flags);
		score2EndOfSeason = computeEndOfSeason(score2, nbGPToDo - i, scoring, flags);
	}

	if (score1 >= score2EndOfSeason)
		return nbGPToDo - i + 1;
	else
		return IMPOSSIBLE;
}

int maxSubArray(unsigned int array[], size_t size, unsigned int start)
{
	int i, index, max;
	max = array[start];
	index = start;

	for (i = start + 1; i < size; i++) {
		if (array[i] >= max) {
			max = array[i];
			index = i;
		}
	}

	return index;
}

void sortArray(unsigned int array[], size_t size)
{
	int i, j = 0, tmp;

	do {
		i = maxSubArray(array, size, j);

		tmp = array[i];
		array[i] = array[j];
		array[j] = tmp;
		j++;
	} while (j < size);
}

void displayArray(unsigned int array[], size_t size)
{
	int i;
	for (i = 0; i < size; i++)
		fprintf(stdout, "\tCompetitor %d = %d points\n", i + 1, array[i]);

	fprintf(stdout, "\n\n");
}

int bestSeason(unsigned int nbGP, unsigned int scoring[], unsigned int nbCompetitors, int flags)
{
        unsigned int *competitorsScore;
	int i, j, champion = NOT_CHAMPION;
	int coef = 1;

        competitorsScore = malloc(sizeof(unsigned int) * nbCompetitors);
        if (!competitorsScore)
                return ERROR;

	memset(competitorsScore, 0, nbCompetitors);

	for (i = 1; i <= nbGP && champion == NOT_CHAMPION; i++) {
                if ((flags & LAST_RACE_DOUBLE) && nbGP - i == 0)
                        coef = 2;

		if (flags & DRIVER) {
			competitorsScore[0] += scoring[0] * coef;

			for (j = 1; scoring[j] > 0; j++)
				competitorsScore[nbCompetitors - j] += scoring[j] * coef;
		} else if (flags & TEAM) {
			competitorsScore[0] += scoring[0] + scoring[1] * coef;

			for (j = 2; scoring[j] > 0; j++)
				competitorsScore[nbCompetitors - j + 1] += scoring[j] * coef;
		}

		printf("Grand prix number %d\n", i);
		sortArray(competitorsScore, nbCompetitors);

		champion = worldChampion(competitorsScore[0], competitorsScore[1], nbGP - i, scoring, flags);
		displayArray(competitorsScore, nbCompetitors);
		
		if (champion != ALREADY_CHAMPION) {
			champion = NOT_CHAMPION;
	        } else {
	                free(competitorsScore);
	                return nbGP - i;
	        }
	}

        free(competitorsScore);
	return IMPOSSIBLE;
}

void help(void)
{
	fprintf(stdout, "Formula1 -y years -g nb_GP -c nb_GP_done -l current_competitor_score -o first_or_second_score -t TEAM|DRIVER\n"
	                "Formula1 -b -y years -g nb_GP -n nb_competitors -t TEAM|DRIVER \n");
}

int main(int argc, char *argv[])
{
        unsigned int year, nbGP, nbGPDone, score1, score2, nbCompetitors;
        int flags, opt;
        unsigned int scoringChoice;
        unsigned int *competitorsScore;
        int result = NOT_CHAMPION;
        enum options mode = COMPUTE_FINAL_CHAMPIONSHIP;

        while ((opt = getopt(argc, argv, "y:g:c:l:o:t:bn:h")) != -1) {
                switch (opt) {
                case 'y':
                   year = atoi(optarg);
                   break;
                case 'g':
                   nbGP = atoi(optarg);
                   break;
                case 'c':
                   nbGPDone = atoi(optarg);
                   break;
                case 'l':
                   score1 = atoi(optarg);
                   break;
                case 'o':
                   score2 = atoi(optarg);
                   break;
                case 't':
                   flags = strncmp(optarg, "DRIVER", sizeof("DRIVER")) ? TEAM : DRIVER;
                   break;
                case 'b':
                   mode = BEST_SEASON;
                   break;
                case 'n':
                   nbCompetitors = atoi(optarg);
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
        
        if (year >= 1950 && year <= 1959)
                scoringChoice = 0;
        else if ((year == 1960) || (year == 1961 && flags & TEAM))
                scoringChoice = 1;
        else if (year >= 1961 && year <= 1990)
                scoringChoice = 2;
        else if (year >= 1991 && year <= 2002)
                scoringChoice = 3;
        else if (year >= 2003 && year <= 2009)
                scoringChoice = 4;
        else
                scoringChoice = 5; 

        if (year == 2014)
                flags += LAST_RACE_DOUBLE;
        
        if (mode == BEST_SEASON)
                result = bestSeason(nbGP, allScoring[scoringChoice], nbCompetitors, flags);
        else
                result = worldChampion(score1, score2, nbGP - nbGPDone, allScoring[scoringChoice], flags);
        
        if (result == IMPOSSIBLE)
                fprintf(stdout, "Your competitor can not be world champion this year\n");
        else if (result == ALREADY_CHAMPION)
                fprintf(stdout, "Your competitor is already world champion this year\n");
        else if (result == ERROR)
                fprintf(stderr, "Can not allocate memory for %d competitors\n", nbCompetitors);
        else
                fprintf(stdout, "Your competitor could be world champion %d grands prix before the end of the season\n", result);

	return EXIT_SUCCESS;
}
