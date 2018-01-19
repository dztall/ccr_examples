#if !defined(__CCR__)
#error "Not supported platform!"
#endif

#include <stdio.h>
#include <ccr.h>

int main()
{
	printf("This should not display\n");
	ccrClearTextOutput();
	printf("This should display\n");

	return 0;
}