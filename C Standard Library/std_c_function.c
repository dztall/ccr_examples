#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YOUR_NAME "world"

#define TEST_TXT_FILE "test.txt"

int main()
{
	char buf[100];
	memset(buf, 0, 100);
	sprintf(buf, "Hello %s!!", YOUR_NAME);

	char buf1[100];
	strcpy(buf1, buf);
	printf("%s\n", buf1);

	FILE *fp = fopen(TEST_TXT_FILE, "wt");
	if (fp)
	{
		fprintf(fp, "Hello world!!\n");
		fclose(fp);
	}
	else
	{
		printf("Error openning file for writing : %s\n", TEST_TXT_FILE);
		return -1;
	}

	char str[80] = { 0, };
	fp = fopen(TEST_TXT_FILE, "rt");
	if (fp)
	{
		fscanf(fp, "%s", str);
		fclose(fp);
	}
	else
	{
		printf("Error openning file for reading : %s\n", TEST_TXT_FILE);
		return -1;
	}

	printf("Read from test.txt : %s", str);

	return 0;
}