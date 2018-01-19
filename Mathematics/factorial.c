#include <stdio.h>

#define ITER_FROM 10
#define ITER_COUNT 6

long long fact(long long x)
{
	if (x > 1)
		return x * fact(x - 1);
	else
		return 1;
}

int main()
{
	for (int i = ITER_FROM; i < ITER_FROM + ITER_COUNT; i++)
		printf("factorial of %d is %lld\n", i, fact(i));

	return 0;
}
