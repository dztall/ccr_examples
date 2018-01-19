#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	int status;
	status = mkdir("testFolder", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (status != 0)
		printf("Failed to create folder.\n");
	else
		printf("Succeed to create folder.\n");

	return 0;
}