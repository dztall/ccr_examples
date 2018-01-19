#include <stdio.h>

#define LOG(...) printf(__VA_ARGS__);

int main()
{
	LOG("__VA_ARGS__ Test OK.");

	return 0;
}