//Calling function by pointer to user defined function.

#include <stdio.h>

void test(int a, int b)
{
	printf("%d,%d\n", a, b);
}

int main()
{
	void(*p) (int, int);

	p = &test;
	p(1, 9);

	test(3, 4);

	return 0;
}