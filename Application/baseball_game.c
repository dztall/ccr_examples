//Authors : Lee Jeong Seop
//CC BY-NC-SA 4.0

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_TRIAL 7

bool isnum(char c)
{
	switch (c)
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return true;
	default:
		return false;
	}
}

void evaluate(char *num_gen, char *num, int *strike_out, int *ball_out)
{
	int strk = 0;
	int ball = 0;

	if (num_gen[0] == num[0])
		strk++;

	if (num_gen[1] == num[1])
		strk++;

	if (num_gen[2] == num[2])
		strk++;

	if (num_gen[0] == num[1] || num_gen[0] == num[2])
		ball++;

	if (num_gen[1] == num[0] || num_gen[1] == num[2])
		ball++;

	if (num_gen[2] == num[1] || num_gen[2] == num[0])
		ball++;

	if (strike_out)
		*strike_out = strk;
	if (ball_out)
		*ball_out = ball;
}

int main()
{
	srand(time(NULL));

	printf("Baseball game\n");

	//Generate random 3-digits number.
	char num_gen[3];
	num_gen[0] = '1' + rand() % 9;
	do
	{
		num_gen[1] = '1' + rand() % 9;
	} while (num_gen[1] == num_gen[0]);

	do
	{
		num_gen[2] = '1' + rand() % 9;
	} while (num_gen[2] == num_gen[0] || num_gen[2] == num_gen[1]);

	//game loop
	int i;
	for (i = 0; i < MAX_TRIAL; i++)
	{
		char num[32] = { 0,0,0,0 };

		printf("Guess 3 numbers? (x to exit) ");
		scanf("%s", num);
		if (num[0] == 'x' || num[0] == 0)
		{
			i = MAX_TRIAL;
			break;
		}

		if (strlen(num) != 3)
		{
			printf("Error : input 3 numbers , %d trials left\n", MAX_TRIAL - 1 - i);
			continue;
		}

		if (!isnum(num[0]) || !isnum(num[1]) || !isnum(num[2]))
		{
			printf("Error : input numbers only , %d trials left\n", MAX_TRIAL - 1 - i);
			continue;
		}

		if (num[0] == num[1] || num[0] == num[2] || num[1] == num[2])
		{
			printf("Error : no duplicated digit allowed , %d trials left\n", MAX_TRIAL - 1 - i);
			continue;
		}

		printf("%s : ", num);

		int strike = 0, ball = 0;
		evaluate(num_gen, num, &strike, &ball);
		if (strike == 3)
		{
			printf("Congratulations!!\n");
			break;
		}
		else
			printf("%d strike %d ball , %d trials left\n", strike, ball, MAX_TRIAL - 1 - i);
	}

	//print result
	if (i == MAX_TRIAL)
	{
		printf("You Lose!!\n");
		printf("The number was %c%c%c.\n", num_gen[0], num_gen[1], num_gen[2]);
	}
	else
		printf("You Win!!\n");

	return 0;
}