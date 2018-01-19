#include <stdio.h>

struct A
{
	int a : 1;
	int b : 2;
	char c : 4;
	char d : 4;
};

int main()
{
	struct A a;

	a.a = 1;
	a.b = -2;
	a.c = -8;
	a.d = 7;

	printf("Bitfield test : %d %d %d %d", a.a, a.b, a.c, a.d);

	return 0;
}