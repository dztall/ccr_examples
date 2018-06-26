//Authors : Lee Jeong Seop
//CC BY-NC-SA 4.0

#include <stdio.h>

int main()
{
	float weight = 70.0f;
	printf("weight in kg? ");
	scanf("%f", &weight);

	float height = 1.77f;
	printf("height in cm? ");
	scanf("%f", &height);

	//10 to 18.4 : underweight
	//18.5 to 24.9 : normal
	//25 to 29.9 : overweight
	//over 30 : obesity
	float bmi;
	bmi = weight / (height*height / 10000.0f);
	printf("BMI is %.2f", bmi);

	return 0;
}