#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

int main()
{
	DIR *pDir;
	struct dirent *ent;

	pDir = opendir("./");
	if (pDir)
	{
		while ((ent = readdir(pDir)) != NULL)
			printf("%s\n", ent->d_name);
		closedir(pDir);
	}
	else
		perror("Failed to open directory.");

	return 0;
}