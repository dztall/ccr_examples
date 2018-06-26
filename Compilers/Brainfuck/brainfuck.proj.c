//Authors : Lee Jeong Seop

//This file does not belong to 'Brainfuck' source distribution.
//This file demonstrates how to run multiple source'ed projects in Mobile C.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <ccr.h>

//#define SHOW_DEBUG_MESSAGES 1

const char *userHeaderSearchPaths[] = { "lib" };
const char *systemHeaderSearchPaths[] = { __DIR__"/include" };
const char *sourceCodePaths[] = { __DIR__"/src" };
const char *sourceExclusionPatterns[] = {};
const char *defines[] = { "#define FOO", "#define BAR" };

void attachSourceFile(CPPuint programID, const char *fileName);
const char *getFileExtension(const char *fileName);
void attachSourceFiles(CPPuint programID);
void exit_handler();

CPPuint programID = 0;

int main()
{
	atexit(exit_handler);

	programID = cppCreateProgram();

	attachSourceFiles(programID);

#if SHOW_DEBUG_MESSAGES
	printf("Compiling and linking...\n");
#endif
	if(cppLinkProgram(programID) != CPP_NO_ERROR)
	{
		printf("Error on linking.\n");
		return -1;
	}

#if SHOW_DEBUG_MESSAGES
	printf("Running...\n");
#endif
	cppRunProgram(programID);

	return 0;
}

void exit_handler()
{
#if SHOW_DEBUG_MESSAGES
	printf("Finished running.\n");
#endif
	if (programID)
		cppDeleteProgram(programID);
}

bool isExcludedSourceFile(const char *sourcePath)
{
	for (int i = 0; i < sizeof(sourceExclusionPatterns) / sizeof(sourceExclusionPatterns[0]); i++)
	{
		const char *sourceExclusionPattern = sourceExclusionPatterns[i];
		if(strstr(sourcePath, sourceExclusionPattern))
			return true;
	}
	return false;
}

void attachSourceFiles(CPPuint programID)
{
	DIR *pDir;
	struct dirent *ent;

	for (int i = 0; i < sizeof(sourceCodePaths) / sizeof(sourceCodePaths[0]); i++)
	{
		const char *sourceFolder = sourceCodePaths[i];

		pDir = opendir(sourceFolder);
		if (pDir)
		{
			while ((ent = readdir(pDir)))
			{
				if (!strcmp(getFileExtension(ent->d_name), "c") && !isExcludedSourceFile(ent->d_name))
				{
					char buf[1024];
					strcpy(buf, sourceFolder);
					strcat(buf, "/");
					strcat(buf, ent->d_name);
					attachSourceFile(programID, buf);
#if SHOW_DEBUG_MESSAGES
					printf("Source file added : %s\n", ent->d_name);
#endif
				}
			}
			closedir(pDir);
		}
		else
		{
			perror("Failed to open directory.");
			break;
		}
	}
}

void attachSourceFile(CPPuint programID, const char *fileName)
{
	CPPuint objectID = cppCreateObject();

	//Set system header search paths.
	for (int i = 0; i < sizeof(systemHeaderSearchPaths) / sizeof(systemHeaderSearchPaths[0]); i++)
		cppObjectAddSystemHeaderSearchPath(objectID, systemHeaderSearchPaths[i]);

	//Set user header search paths.
	for (int i = 0; i < sizeof(userHeaderSearchPaths) / sizeof(userHeaderSearchPaths[0]); i++)
		cppObjectAddUserHeaderSearchPath(objectID, userHeaderSearchPaths[i]);

	//Macro definitions...
	for (int i = 0; i < sizeof(defines) / sizeof(defines[0]); i++)
		cppObjectAddPrologue(objectID, defines[i]);

	//Appends code at the end of source file.
	//cppObjectAddEpilogue(objectID, "//Blah blah...");

	cppObjectSourceFile(objectID, fileName);
	if(cppCompileObject(objectID) != CPP_NO_ERROR)
		exit(-1);
	cppAttachObject(programID, objectID);
	cppDeleteObject(objectID);
}

const char *getFileExtension(const char *fileName)
{
	const char *ext = strrchr(fileName, '.');
	if (ext)
		return ext + 1;
	return "";
}