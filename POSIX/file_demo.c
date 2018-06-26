#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "file.txt"

int main()
{
	int fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{
		printf("Error openning file : '%s'.\n", FILENAME);
		return 1;
	}
	
	const char *buf = "Hello world!!";
	write(fd, buf, strlen(buf));
	
	close(fd);
	
	printf("File created and written : '%s'.\n", FILENAME);
	
	int fdRead = open(FILENAME, O_RDWR);
	if(fdRead < 0)
	{
		printf("Error openning file for reading : '%s'.\n", FILENAME);
		return 2;
	}
	
	const char *bufRead[1024];
	read(fdRead, bufRead, sizeof(bufRead));
	
	close(fdRead);
	
	printf("File read : '%s', '%s'.\n", FILENAME, bufRead);

	return 0;
}
