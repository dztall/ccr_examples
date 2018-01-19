#include <stdio.h>
#include <string.h>

char *strrev(char *str)
{
	char *p1, *p2;

	if (!str || !*str)
		return str;

	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		char t = *p1;
		*p1 = *p2;
		*p2 = t;
	}
	return str;
}

int main()
{
	char a[10];

	strcpy(a, "1234");
	printf("Before strrev : %s\n", a);

	strrev(a);
	printf("After strrev : %s\n", a);

	return 0;
}