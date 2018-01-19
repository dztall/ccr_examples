//assert(...) function example code.

#include <assert.h>

int main()
{
	int a = 1 / 2;
	assert(a == 0);
	//For assert working test.
	assert(a == 1 && "False assertion to assert assert function works.");

	return 0;
}