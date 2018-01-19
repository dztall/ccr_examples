//Original source code found at : http://www.cplusplus.com/reference/cstdio/getchar/

//getchar example : typewriter.
#include <stdio.h>

int main()
{
	int c;

	puts("Enter text. Include a dot ('.') in a sentence to exit:");
	do
	{
		c = getchar();
		putchar(c);
	} while (c != '.');

	return 0;
}
