//Authors : Lee Jeong Seop
//CC BY-NC-SA 4.0

//Prints pyramid like shape using for loops.

#include <stdio.h>

#define HEIGHT 20

int main()
{
	for (int i = 1; i < HEIGHT * 2; i++)
	{
		if (i % 2 == 0)
			continue;

		for (int k = 0; k < (HEIGHT * 2 - i) / 2; k++)
			printf(" ");

		for (int j = 1; j <= i; j++)
			printf("*");
		printf("\n");
	}

	return 0;
}