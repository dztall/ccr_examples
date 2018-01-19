#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M 3 /* n of rows in first matrix */
#define N 4 /* n of cols in first and of rows in 2nd */
#define P 5 /* n of cols on 2nd matrix */

void multmatrix(int a[M][N], int b[N][P], int c[M][P])
{
	int i, j, k;

	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < P; ++j)
		{
			c[i][j] = 0.0;
			for (k = 0; k < N; ++k)
				c[i][j] = c[i][j] + a[i][k] * b[k][j];
		}
	}
}

int main()
{
	int i, j;
	int mat1[M][N] = { {1,2,2,2},{2,-3,6,4},{8,1,0,-3} };
	int mat2[N][P] = { {1,1,1,0,3},{2,3,1,6,2},{1,-1,-1,8,3},{0,1,2,3,4} };
	int matprod[M][P];

	/* do the multiplication */
	multmatrix(mat1, mat2, matprod);

	/* display the resulting matrix */
	for (i = 0; i < M; ++i)
	{
		for (j = 0; j < P; ++j)
			printf("%4d ", matprod[i][j]);
		printf("\n");
	}

	return (0);
}