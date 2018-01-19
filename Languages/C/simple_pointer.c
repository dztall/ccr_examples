//Simple pointer test.

#include <stdio.h>

int main()
{
	int a, *b;

	a = 1;
	printf("a=%d\n", a);

	b = &a;
	*b = 2;
	printf("a=%d\n", a);

	return 0;
}