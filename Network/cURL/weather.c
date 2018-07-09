//Original source code found at : https://theptrk.com/2018/07/08/weather-reports-with-curl/

#include <stdio.h>
#include <stdlib.h>

int main()
{
	char city[512] = "Seoul";
	printf("Your city? ");
	scanf("%s", city);

	char command[512] = "";
	sprintf(command, "curl wttr.in/~%s?0", city);
	system(command);

	return 0;
}
