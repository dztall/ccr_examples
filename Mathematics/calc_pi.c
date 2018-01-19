#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

//For more accurate result increase the value.
#define ITERATION 99999

int main()
{
	double pi = 0.0;
	int n = ITERATION;

	for (int i = 1; i <= n; i++)
	{
		pi += sqrt((double)1.0 -
			pow(((double)(2.0*i) / n - (double)1.0), 2))*
			((double)2.0 / n);
	}
	pi *= (double)2.0;
	printf("%f\n", pi);

	return 0;
}