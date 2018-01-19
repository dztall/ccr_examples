//Original source code found at : http://www.cprogramming.com/cgi-bin/source/source.cgi?action=Category&CID=1

/*This program prints a calendar for a year specified. The user enters a
year for the calendar and the programs automatically prints the calendar
in text format. The codes are: day_code (0 = Sun, 1 = Mon, etc.)
leap_year (0 = no leap year, 1 = leap year)   */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

int get_year(void)
{
	int year = -1;
	printf("Enter a year: ");
	scanf("%d", &year);
	if (year == -1)
	{
		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		year = timeinfo->tm_year + 1900;
	}
	return year;
}

int get_day_code(int year)
{
	int day_code;
	int x1, x2, x3;
	x1 = (year - 1.) / 4.0;
	x2 = (year - 1.) / 100.;
	x3 = (year - 1.) / 400.;
	day_code = (year + x1 - x2 + x3) % 7;
	return day_code;
}

int get_leap_year(int year)
{
	if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
		return TRUE;
	else
		return FALSE;
}

void print_calendar(int year, int day_code, int leap_year) //function header
{
	int days_in_month;		/* number of days in month currently being printed */
	int day;				/* counter for day of month */
	int month;				/* month = 1 is Jan, month = 2 is Feb, etc. */

	printf("\t\t\t\t%d", year);

	for (month = 1; month <= 12; month++)
	{
		switch (month)
		{
			/* print name and set days_in_month */
		case 1:
			printf("\n\nJanuary");
			days_in_month = 31;
			break;
		case 2:
			printf("\n\nFebruary");
			days_in_month = leap_year ? 29 : 28;
			break;
		case 3:
			printf("\n\nMarch");
			days_in_month = 31;
			break;
		case 4:
			printf("\n\nApril");
			days_in_month = 30;
			break;
		case 5:
			printf("\n\nMay");
			days_in_month = 31;
			break;
		case 6:
			printf("\n\nJune");
			days_in_month = 30;
			break;
		case 7:
			printf("\n\nJuly");
			days_in_month = 31;
			break;
		case 8:
			printf("\n\nAugust");
			days_in_month = 31;
			break;
		case 9:
			printf("\n\nSeptember");
			days_in_month = 30;
			break;
		case 10:
			printf("\n\nOctober");
			days_in_month = 31;
			break;
		case 11:
			printf("\n\nNovember");
			days_in_month = 30;
			break;
		case 12:
			printf("\n\nDecember");
			days_in_month = 31;
			break;
		}

		printf("\n\nSun\tMon\tTue\tWed\tThu\tFri\tSat\n");

		/* advance printer to correct position for first date */
		for (day = 1; day < day_code + 1; day++)
			printf("\t");

		/* print the dates for one month */
		for (day = 1; day <= days_in_month; day++)
		{
			printf("%2d", day);
			if ((day + day_code) % 7 > 0) /* before Sat? */
										 /* move to next day in same week */
				printf("\t");
			else if (day != days_in_month)  /* skip to next line to start with Sun */
				printf("\n");
		}

		/* set day_code for next month to begin */
		day_code = (day_code + days_in_month) % 7;
	}
}

int main()
{
	int year, day_code, leap_year;

	year = get_year();
	day_code = get_day_code(year);
	leap_year = get_leap_year(year);
	print_calendar(year, day_code, leap_year);

	return 0;
}