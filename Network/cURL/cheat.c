//Original source code found at : https://github.com/chubin/cheat.sh

#include <stdio.h>
#include <stdlib.h>

int main()
{
	char lang[512] = "c";
	printf("Programming language? ");
	scanf("%s", lang);

	char keywords[512] = "append+file";
	printf("Keywords?\nex) append+file ");
	scanf("%s", keywords);

	char command[512] = "";
	sprintf(command, "curl cht.sh/%s/%s", city, keywords);
	system(command);

	return 0;
}
