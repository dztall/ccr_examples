//Original source code found at : http://www.softwareandfinance.com/Turbo_C/TowerOfHanoi_Algorithm.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _MyStack
{
	int *m_data;
	int m_numElements;
} MyStack;

static int movecount = 0;
static int countA = 0;
static int countB = 0;
static int countC = 0;

static MyStack *A = 0;
static MyStack *B = 0;
static MyStack *C = 0;

int top(MyStack *s);
int size(MyStack *s);

int push(MyStack *s, int data)
{
	if (s->m_data == NULL) // root node
	{
		s->m_numElements = 1;
		s->m_data = (int*)malloc(sizeof(int));
	}
	else
	{
		s->m_numElements++;
		s->m_data = realloc(s->m_data, s->m_numElements * sizeof(int));
		memmove(&s->m_data[1], s->m_data, (s->m_numElements - 1) * sizeof(int));
	}

	s->m_data[0] = data;
	return 1;
}

int pop(MyStack *s)
{
	int result = -1;
	if (s->m_data == NULL) // root node
	{
		s->m_numElements = 0;
		return result;
	}
	else
	{
		result = top(s);
		if (s->m_numElements == 1)
		{
			// last item
			s->m_numElements = 0;
			free(s->m_data);
			s->m_data = NULL;
		}
		else
		{
			s->m_numElements--;
			memmove(s->m_data, &s->m_data[1], s->m_numElements * sizeof(int));
			s->m_data = (int*)realloc(s->m_data, s->m_numElements * sizeof(int));
		}
	}
	return result;
}

int top(MyStack *s)
{
	if (s->m_numElements > 0)
		return s->m_data[0];
	return 0;
}

void ClearStack(MyStack *s)
{
	free(s->m_data);
	s->m_data = NULL;
	s->m_numElements = 0;
}

int size(MyStack *s)
{
	return s->m_numElements;
}

void PrintStack(MyStack *s)
{
	int i = 0;
	printf("[");
	for (i = s->m_numElements - 1; i >= 0; i--)
		printf("%d", s->m_data[i]);
	printf("]");
}

void PrintStacks()
{
	if (countA != A->m_numElements ||
		countB != B->m_numElements ||
		countC != C->m_numElements)
	{
		int diffA = A->m_numElements - countA;
		int diffB = B->m_numElements - countB;
		int diffC = C->m_numElements - countC;
		if (diffA == 1)
		{
			if (diffB == -1)
				printf("Move Disc %d From B To A", top(A));
			else
				printf("Move Disc %d From C To A", top(A));
		}
		else if (diffB == 1)
		{
			if (diffA == -1)
				printf("Move Disc %d From A To B", top(B));
			else
				printf("Move Disc %d From C To B", top(B));
		}
		else //if (diffC == 1)
		{
			if (diffA == -1)
				printf("Move Disc %d From A To C", top(C));
			else
				printf("Move Disc %d From B To C", top(C));
		}
		countA = A->m_numElements;
		countB = B->m_numElements;
		countC = C->m_numElements;
		printf("\n");
	}

	PrintStack(A);
	printf(" , ");
	PrintStack(B);
	printf(" , ");
	PrintStack(C);
	printf(" , ");
}

void Solve2DiscsTOH(MyStack *source, MyStack *temp, MyStack *dest)
{
	push(temp, pop(source));
	movecount++;
	PrintStacks();

	push(dest, pop(source));
	movecount++;
	PrintStacks();

	push(dest, pop(temp));
	movecount++;
	PrintStacks();
}

int SolveTOH(int nDiscs, MyStack *source, MyStack *temp, MyStack *dest)
{
	if (nDiscs <= 4)
	{
		if ((nDiscs % 2) == 0)
		{
			Solve2DiscsTOH(source, temp, dest);
			nDiscs = nDiscs - 1;
			if (nDiscs == 1)
				return 1;

			push(temp, pop(source));
			movecount++;
			PrintStacks();
			//New source is dest, new temp is source, new dest is temp;
			Solve2DiscsTOH(dest, source, temp);

			push(dest, pop(source));
			movecount++;
			PrintStacks();
			//New source is temp, new temp is source, new dest is dest;
			SolveTOH(nDiscs, temp, source, dest);
		}
		else
		{
			if (nDiscs == 1)
				return 0;
			Solve2DiscsTOH(source, dest, temp);
			nDiscs = nDiscs - 1;
			push(dest, pop(source));
			movecount++;
			PrintStacks();
			Solve2DiscsTOH(temp, source, dest);
		}
		return 1;
	}
	else if (nDiscs >= 5)
	{
		SolveTOH(nDiscs - 2, source, temp, dest);
		push(temp, pop(source));
		movecount++;
		PrintStacks();
		SolveTOH(nDiscs - 2, dest, source, temp);
		push(dest, pop(source));
		movecount++;
		PrintStacks();
		SolveTOH(nDiscs - 1, temp, source, dest);
	}
	return 1;
}

int main()
{
	int sz, i, maxdisc;
	A = calloc(1, sizeof(MyStack));
	B = calloc(1, sizeof(MyStack));
	C = calloc(1, sizeof(MyStack));

	while (1)
	{
		printf("Enter the number of discs (-1 to exit): ");
		scanf("%d", &maxdisc);
		if (maxdisc == -1)
			break;

		if (maxdisc < 2 || maxdisc > 9)
		{
			printf("Enter between 2 - 9\n");
			continue;
		}

		movecount = 0;

		ClearStack(A);
		ClearStack(B);
		ClearStack(C);

		for (i = maxdisc; i >= 1; i--)
			push(A, i);

		countA = A->m_numElements;
		countB = B->m_numElements;
		countC = C->m_numElements;

		PrintStacks();
		SolveTOH(maxdisc, A, B, C);
		printf("Total Moves = %d\n", movecount);
	}

	ClearStack(A);
	ClearStack(B);
	ClearStack(C);

	free(A);
	free(B);
	free(C);

	return 0;
}

