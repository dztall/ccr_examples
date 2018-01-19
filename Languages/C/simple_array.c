//Simple array test.

#include <stdio.h>

int main()
{
	int arr[101];

	for (int i = 0; i <= 100; i++)
		arr[i] = i;

	int sum = 0;
	for (int i = 0; i <= 100; i++)
		sum += arr[i];

	printf("sum=%d\n", sum);

	return 0;
}