//Authors : Lee Jeong Seop

//This file demonstrates how to run multiple source'ed c++ projects in Mobile C.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <ccr.h>

//#define SHOW_DEBUG_MESSAGES 1

const char *userHeaderSearchPaths[] = { "src" };
const char *systemHeaderSearchPaths[] = {};
const char *sourceCodePaths[] = { __DIR__"/src" };
const char *sourceExclusionPatterns[] = {};
const char *defines[] = {"FOO=1", "BAR" };

void attachSourceFile(CCRuint programID, const char *fileName);
const char *getFileExtension(const char *fileName);
void attachSourceFiles(CCRuint programID);
void exit_handler();

CCRuint programID = 0;

int main()
{
	atexit(exit_handler);

	programID = ccrCreateClangProgram();
	ccrClangProgramTargetFile(programID, __FILE__ ".bc");
	
	attachSourceFiles(programID);
		
#if SHOW_DEBUG_MESSAGES
	printf("Compiling and linking...\n");
#endif
	if(ccrLinkClangProgram(programID) != CCR_NO_ERROR)
	{
		printf("Error on linking.\n");
		return -1;
	}

#if SHOW_DEBUG_MESSAGES
	printf("Running...\n");
#endif
	ccrRunClangProgram(programID, false);

	return 0;
}

void exit_handler()
{
#if SHOW_DEBUG_MESSAGES
	printf("Finished running.\n");
#endif
	if(programID)
		ccrClangProgramDeleteClangObjectTargetFiles(programID);
	if (programID)
		ccrDeleteClangProgram(programID);
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

void attachSourceFiles(CCRuint programID)
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
				if ((!strcmp(getFileExtension(ent->d_name), "c") || !strcmp(getFileExtension(ent->d_name), "cpp") || !strcmp(getFileExtension(ent->d_name), "cc") || !strcmp(getFileExtension(ent->d_name), "c++")) && !isExcludedSourceFile(ent->d_name))
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

void attachSourceFile(CCRuint programID, const char *fileName)
{
	CCRuint objectID = ccrCreateClangObject();

	//Set system header search paths.
	for (int i = 0; i < sizeof(systemHeaderSearchPaths) / sizeof(systemHeaderSearchPaths[0]); i++)
		ccrClangObjectAddOption(objectID, "-I", systemHeaderSearchPaths[i]);

	//Set user header search paths.
	for (int i = 0; i < sizeof(userHeaderSearchPaths) / sizeof(userHeaderSearchPaths[0]); i++)
		ccrClangObjectAddOption(objectID, "-I", userHeaderSearchPaths[i]);

	//Macro definitions...
	for (int i = 0; i < sizeof(defines) / sizeof(defines[0]); i++)
		ccrClangObjectAddOption(objectID, "-D", defines[i]);

	ccrClangObjectSourceFile(objectID, fileName);
	if(ccrCompileClangObject(objectID) != CCR_NO_ERROR)
		exit(-1);
	ccrAttachClangObject(programID, objectID);
	ccrDeleteClangObject(objectID);
}

const char *getFileExtension(const char *fileName)
{
	const char *ext = strrchr(fileName, '.');
	if (ext)
		return ext + 1;
	return "";
}
