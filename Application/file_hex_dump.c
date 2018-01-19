//Original source code found at : http://c2.com/cgi/wiki?HexDumpInManyProgrammingLanguages

#include <stdio.h>
#include <stdlib.h>

#define TEST_FILE "file_hex_dump.c"

void hexdump(unsigned char *buffer, unsigned long index, unsigned long width)
{
	unsigned long i;
	for (i = 0; i < index; i++)
		printf("%02x\t", buffer[i]);

	for (unsigned long spacer = index; spacer < width; spacer++)
		printf("\t");
	printf(": ");
	for (i = 0; i < index; i++)
	{
		if (buffer[i] < 32 || buffer[i] >= 127)
			printf(".");
		else
			printf("%c", buffer[i]);
	}
	printf("\n");
}

int hexdump_file(FILE *infile, unsigned long start, unsigned long stop, unsigned long width)
{
	char ch;
	unsigned long f_index = 0;
	unsigned long bb_index = 0;
	unsigned char *byte_buffer = malloc(width);
	if (byte_buffer == NULL)
	{
		printf("Could not allocate memory for byte_buffer\n");
		return -1;
	}
	while (!feof(infile))
	{
		ch = getc(infile);
		if ((f_index >= start) && (f_index <= stop))
		{
			byte_buffer[bb_index] = ch;
			bb_index++;
		}
		if (bb_index >= width)
		{
			hexdump(byte_buffer, bb_index, width);
			bb_index = 0;
		}
		f_index++;
	}
	if (bb_index)
		hexdump(byte_buffer, bb_index, width);
	fclose(infile);
	free(byte_buffer);
	return 0;
}

int main()
{
	char fileName[512] = TEST_FILE;
	printf("File name ( %s )? ", TEST_FILE);
	scanf("%s", fileName);

	int start = 0, length = 100, width = 8;
	printf("Start , length ( %d %d )? ", start, start + length);
	scanf("%d %d", &start, &length);

	FILE *infile = fopen(fileName, "rb");
	if (infile == (FILE *)NULL)
	{
		printf("Error opening input file %s\n", fileName);
		return 0;
	}
	printf("\nHex display of %s ( %d - %d ).\n\n", fileName, start, start + length);
	int result = hexdump_file(infile, start, start + length, width);
	return result;
}