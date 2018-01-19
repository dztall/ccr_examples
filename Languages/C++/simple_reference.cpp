#include <stdio.h>

void func(char &a, short &b, int &c, long long &d, unsigned char &e,
	unsigned short &f, unsigned int &g, unsigned long long &h,
	float &i, double &j)
{
	if (a == 1)
		a = 2;
	if (b == 10)
		b = 20;
	if (c == 100)
		c = 200;
	if (d == 1000)
		d = 2000;
	if (e == 10)
		e = 20;
	if (f == 100)
		f = 200;
	if (g == 1000)
		g = 2000;
	if (h == 10000)
		h = 20000;
	if (i == 1.234f)
		i = 2.345f;
	if (j == 3.456)
		j = 4.567;
}

int main()
{
	char a = 1;
	short b = 10;
	int c = 100;
	long long d = 1000;
	unsigned char e = 10;
	unsigned short f = 100;
	unsigned int g = 1000;
	unsigned long long h = 10000;
	float i = 1.234f;
	double j = 3.456;

	func(a, b, c, d, e, f, g, h, i, j);

	int k = 1;
	int &l = k;
	l = 2;

	printf("All should print 1.\n");
	printf("%d %d %d %d %d %d %d %d %d %d %d %d\n",
		a == 2, b == 20, c == 200, d == 2000, e == 20, f == 200, g == 2000, h == 20000, i == 2.345f, j == 4.567,
		k == 2, l == 2);

	return 0;
}