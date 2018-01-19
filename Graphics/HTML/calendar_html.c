//HTML version of calendar.c

#if !defined(__CCR__)
#error "Not supported platform!"
#endif

/*This program prints a calendar for a year specified. The user enters a
year for the calendar and the programs automatically prints the calendar
in text format.
The codes are: day_code (0 = Sun, 1 = Mon, etc.)
leap_year (0 = no leap year, 1 = leap year)   */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//For html() function.
#include <ccr.h>

#define TRUE 1
#define FALSE 0

int get_year()
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
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return TRUE;
	else
		return FALSE;
}

void print_calendar(int year, int day_code, int leap_year)
{
	/* number of days in month currently being printed */
	int  days_in_month;
	/* counter for day of month */
	int day;
	/* month = 1 is Jan, month = 2 is Feb, etc. */
	int month;

	html("<html>");
	html("<head>\n");
	html("<meta name='viewport' "
		"content='width=320;"
		"maximum-scale=2.0;"
		"minimum-scale=0.2;"
		"initial-scale=1.0'/>");
	html("</head>\n");
	html("<body bgcolor=#%06lx>\n",
		(rand() % 255) | (rand() % 255) << 8 | (rand() % 255) << 16);

	html("                   %d", year);

	for (month = 1; month <= 12; month++)
	{
		html("<table border=\"1\">");
		html("<tr><th>");
		switch (month)
		{
			/* print name and set days_in_month */
		case 1:
			html("\n\nJanuary");
			days_in_month = 31;
			break;
		case 2:
			html("\n\nFebruary");
			days_in_month = leap_year ? 29 : 28;
			break;
		case 3:
			html("\n\nMarch");
			days_in_month = 31;
			break;
		case 4:
			html("\n\nApril");
			days_in_month = 30;
			break;
		case 5:
			html("\n\nMay");
			days_in_month = 31;
			break;
		case 6:
			html("\n\nJune");
			days_in_month = 30;
			break;
		case 7:
			html("\n\nJuly");
			days_in_month = 31;
			break;
		case 8:
			html("\n\nAugust");
			days_in_month = 31;
			break;
		case 9:
			html("\n\nSeptember");
			days_in_month = 30;
			break;
		case 10:
			html("\n\nOctober");
			days_in_month = 31;
			break;
		case 11:
			html("\n\nNovember");
			days_in_month = 30;
			break;
		case 12:
			html("\n\nDecember");
			days_in_month = 31;
			break;
		}
		html("</th></tr>");
		html("<tr><th>");
		html("<table border=\"1\">");
		html("<tr>");
		html("<th>Sun</th><th>Mon</th><th>Tue</th>"
			"<th>Wed</th><th>Thu</th><th>Fri</th><th>Sat</th>\n");
		html("</tr>");
		html("<tr>");

		/* advance printer to correct position for first date */
		for (day = 1; day <= day_code; day++)
			html("<th> </th>");

		/* print the dates for one month */
		for (day = 1; day <= days_in_month; day++)
		{
			html("<th>%2d</th>", day);

			/* skip to next line to start with Sun */
			if (!((day + day_code) % 7 > 0)) /* before Sat? */
				html("</tr><tr>");
		}

		/* set day_code for next month to begin */
		day_code = (day_code + days_in_month) % 7;

		html("</tr>");
		html("</table>");

		html("</th></tr>");
		html("</table>");
	}

	html("</th></tr></table>\n");
	html("</body>\n");
	html("</html>");
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