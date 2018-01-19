#include <stdio.h>

int main()
{
	char firstName[64] = {0, }, lastName[64] = {0, };

	printf("Your First Name? ");
	scanf("%s", firstName);

	printf("Your Last Name? ");
	scanf("%s", lastName);

	printf("Hello, %s %s!!\n", firstName, lastName);

	return 0;
}