//Original source code found at : https://stackoverflow.com/questions/3585846/color-text-in-terminal-applications-in-unix

//For detailed information, search internet with 'ANSI Escape sequences' keyword.

#include <stdio.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33;40m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37;40m"
#define RESET "\x1B[0m"
#define CLEARSCREEN "\x1b[2J"

int main()
{
	printf("This should not display.");
	printf(CLEARSCREEN);

	printf("\x1b[5;20H");
	printf(RED "red\n" RESET);
	printf("\x1b[6;20H");
	printf(GRN "green\n" RESET);
	printf("\x1b[7;20H");
	printf(YEL "yellow\n" RESET);
	printf("\x1b[8;20H");
	printf(BLU "blue\n" RESET);
	printf("\x1b[9;20H");
	printf(MAG "magenta\n" RESET);
	printf("\x1b[10;20H");
	printf(CYN "cyan\n" RESET);
	printf("\x1b[11;20H");
	printf(WHT "white\n" RESET);

	return 0;
}
