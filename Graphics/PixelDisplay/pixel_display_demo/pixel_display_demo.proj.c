//Authors : Lee Jeong Seop

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#include <ccr.h>

//#define SHOW_DEBUG_MESSAGES

const char *userHeaderSearchPaths[] = { "lib" };
const char *sourceCodePaths[] = { __DIR__"/src",__DIR__"/lib" };
const char *defines[] = { "#define FOO","#define BAR" };

void attachSourceFile(CPPuint programID, const char *fileName);
const char *getFileExtension(const char *fileName);

CPPuint programID = 0;

void exit_handler()
{
#ifdef SHOW_DEBUG_MESSAGES
	printf("Finished running.\n");
#endif
	if (programID)
		cppDeleteProgram(programID);
}

int main()
{
	atexit(exit_handler);

	programID = cppCreateProgram();

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
				if (!strcmp(getFileExtension(ent->d_name), "c"))
				{
					char buf[1024];
					strcpy(buf, sourceFolder);
					strcat(buf, "/");
					strcat(buf, ent->d_name);
					attachSourceFile(programID, buf);
#ifdef SHOW_DEBUG_MESSAGES
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

#ifdef SHOW_DEBUG_MESSAGES
	printf("Compiling and linking...\n");
#endif
	cppLinkProgram(programID);

#ifdef SHOW_DEBUG_MESSAGES
	printf("Running...\n");
#endif
	cppRunProgram(programID);

	return 0;
}

void attachSourceFile(CPPuint programID, const char *fileName)
{
	CPPuint objectID = cppCreateObject();

	//Set system header search paths.
	//cppObjectAddSystemHeaderSearchPath(objectID, __DIR__"/your_system_header_folder");

	//Set user header search paths.
	for (int i = 0; i < sizeof(userHeaderSearchPaths) / sizeof(userHeaderSearchPaths[0]); i++)
		cppObjectAddUserHeaderSearchPath(objectID, userHeaderSearchPaths[i]);

	//Macro definitions...
	for (int i = 0; i < sizeof(defines) / sizeof(defines[0]); i++)
		cppObjectAddPrologue(objectID, defines[i]);

	//Appends code at the end of source file.
	//cppObjectAddEpilogue(objectID, "//Blah blah...");

	cppObjectSourceFile(objectID, fileName);
	cppCompileObject(objectID);
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
