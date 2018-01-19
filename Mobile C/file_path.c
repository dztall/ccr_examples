#if !defined(__CCR__)
#error "Not supported platform!"
#endif

//Included for getcwd function.
#include <unistd.h>

#include <stdio.h>

//Included for ccrGetAbsolutePath function.
#include <ccr.h>

int main()
{
	char *cwd = getcwd(NULL, 0);
	printf("Current Directory=%s\n", cwd);
	free(cwd);

	//char *ccrGetAbsolutePath(const char *relativePath,char *buf,int bufSize);
	//If buf is NULL, returns internally malloc'd memory. So, don't forget to free memory.
	//If buf is not NULL, returns buf.

	char *fullPath = ccrGetAbsolutePath("file_path.c", NULL, 0);
	printf("\nAbsolute Path=%s\n", fullPath);
	free(fullPath);

	return 0;
}