#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NB_MONTHS 12

/* Date start from Saturday 01/01/0001 */
const size_t NB_DAYS_PER_WEEK = 7;
const size_t NB_DAYS_PER_YEAR = 365;
const size_t SATURDAY_OFFSET = 5;
const size_t JULIAN_YEAR_END = 1582;
const size_t LEAP_YEAR_INT = 4;
const size_t MARCH = 3;

bool isLeapYear(size_t year)
{
        return (year % LEAP_YEAR_INT == 0 && (year % 100 != 0 || year % 400 == 0));
}

size_t nbDaysSinceYear1(size_t day, size_t month, size_t year)
{
        size_t monthDuration[NB_MONTHS] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
        size_t nbDays, i;
        
        nbDays = day - 1;

        for (i = 0; i < month - 1; i++)
                nbDays += monthDuration[i];

        if (year > JULIAN_YEAR_END) {
                nbDays += NB_DAYS_PER_YEAR * (year - 1);
                nbDays += year / LEAP_YEAR_INT;
                nbDays -= year / 100;
                nbDays += year / 400;
                nbDays += 2;
        } else {
                nbDays += NB_DAYS_PER_YEAR * (year - 1);
                nbDays += year / LEAP_YEAR_INT;
        }

        if (isLeapYear(year) && month < MARCH)
                nbDays -= 1;

        return nbDays;
}

int main(int argc, char *argv[])
{
        char *date;
        size_t day, month, year, nbDays;
        int ret;
        char *dayNames[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
                             "Saturday", "Sunday" };

        if (argc != 2) {
                fprintf(stderr, "date 17/02/1991\n");
                return EXIT_FAILURE;
        }

        date = argv[1];
        ret = sscanf(date, "%zu/%zu/%zu", &day, &month, &year);
        if (ret != 3) {
                fprintf(stderr, "You must pass as argument a valid date as: day/month/year\n");
                return EXIT_FAILURE;
        }

        nbDays = nbDaysSinceYear1(day, month, year);
        
        fprintf(stdout, "%s\n", dayNames[(nbDays + SATURDAY_OFFSET) % NB_DAYS_PER_WEEK]);

        return EXIT_SUCCESS;
}
