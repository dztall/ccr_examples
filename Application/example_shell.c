//Authors : Lee Jeong Seop
//CC BY-NC-SA 4.0

//example_shell  - Example shell command line interpreter.

#ifdef __GNUC__

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef __CCR__
#include <ccr.h> //for ccrGetEvent()
#endif

void PrintHelp()
{
	printf("\n");
	printf("list(ls) : Lists files and directories.\n");
	printf("root(cd /) : Goes to root directory.\n");
	printf("home(cd ~) : Goes to home directory.\n");
	printf("up(cd ..) : Goes to parent directory.\n");
	printf("down(cd) [dir] : Goes to child directory.\n");
	printf("file [file] : Creates file.\n");
	printf("dir [dir] : Creates directory.\n");
	printf("del(rm) [file/dir] : Removes file or directory.\n");
	printf("copy(cp) [src file/dir] [dst dir] : Copies file or directory.\n");
	printf("move(mv) [src file/dir] [dst dir] : Moves file or directory.\n");
	printf("rename [from file/dir] [to file/dir] : Renames file or directory name.\n");
	printf("clear(cls) : Clear screen.\n");
	printf("\n");
}

void Chomp(char *str)
{
	size_t len = strlen(str);
	if (len)
		str[len - 1] = 0;
}

char currentDirectory[512];

bool is_directory(const char *localFileName)
{
	char *buf;
	size_t len;
	char *currentDirectory1 = currentDirectory[0] ? currentDirectory : ".";

	len = strlen(currentDirectory1) + strlen(localFileName) + 2;
	buf = malloc(len);

	if (buf)
	{
		snprintf(buf, len, "%s/%s", currentDirectory1, localFileName);

		struct stat statbuf;
		if (!stat(buf, &statbuf))
		{
			free(buf);
			if (S_ISDIR(statbuf.st_mode))
				return true;
		}
		else
			free(buf);
	}
	return false;
}

void list_command()
{
	DIR *dir;
	struct dirent *ent;
	char *currentDirectory1 = currentDirectory[0] ? currentDirectory : ".";
	if ((dir = opendir(currentDirectory1)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (is_directory(ent->d_name))
				printf("%s/\n", ent->d_name);
			else
				printf("%s\n", ent->d_name);
		}
		closedir(dir);
	}
	else
	{
		perror(currentDirectory1);
		return;
	}
}

void clear_command()
{
	system("clear");
}

char *strstr_backward(char *str, const char *str_find)
{
	char *str_found_backward = NULL;
	char *str_found = NULL;
	size_t position = 0;

	while ((str_found = strstr(str + position, str_find)) != NULL)
	{
		str_found_backward = str_found;
		position = (str_found - str) + 1;
	}

	return str_found_backward;
}

void root_command()
{
	strcpy(currentDirectory, getenv("HOME"));
}

void home_command()
{
	strcpy(currentDirectory, __DOC_DIR__);
}

void up_command()
{
	char *str_found = NULL;
	if ((str_found = strstr_backward(currentDirectory, "/")) != NULL)
		*str_found = 0;
	else
		currentDirectory[0] = 0;
}

void down_command(const char *childDir)
{
	if (currentDirectory[0])
		strcat(currentDirectory, "/");
	strcat(currentDirectory, childDir);
}

void file_command(const char *fileName)
{
	char filePath[512];
	strcpy(filePath, currentDirectory);

	if (filePath[0])
		strcat(filePath, "/");
	strcat(filePath, fileName);

	FILE *fp = fopen(filePath, "w");
	if (fp)
		fclose(fp);
}

void dir_command(const char *dirName)
{
	char dirPath[512];
	strcpy(dirPath, currentDirectory);

	if (dirPath[0])
		strcat(dirPath, "/");
	strcat(dirPath, dirName);

	mkdir(dirPath, 0777);
}

int remove_file_or_directory(const char *path)
{
	struct stat statbuf;
	if (!stat(path, &statbuf))
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			DIR *d = opendir(path);
			size_t path_len = strlen(path);
			int r = -1;

			if (d)
			{
				struct dirent *p;

				r = 0;

				while (!r && (p = readdir(d)))
				{
					int r2 = -1;
					char *buf;
					size_t len;

					if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
						continue;

					len = path_len + strlen(p->d_name) + 2;
					buf = malloc(len);

					if (buf)
					{
						snprintf(buf, len, "%s/%s", path, p->d_name);

						r2 = remove_file_or_directory(buf);

						free(buf);
					}

					r = r2;
				}

				closedir(d);
			}

			if (!r)
				r = rmdir(path);

			return r;
		}
		else
			return remove(path);
	}
	return -1;
}

void del_command(const char *fileDirName)
{
	char fileDirPath[512];
	strcpy(fileDirPath, currentDirectory);

	if (fileDirPath[0])
		strcat(fileDirPath, "/");
	strcat(fileDirPath, fileDirName);

	remove_file_or_directory(fileDirPath);
}

int copy_file(const char *sourceFile, const char *destPath)
{
	int ch;
	FILE *source, *target;

	source = fopen(sourceFile, "r");

	if (source == NULL)
	{
		perror(sourceFile);
		return -1;
	}

	target = fopen(destPath, "w");

	if (target == NULL)
	{
		fclose(source);
		perror(destPath);
		return -1;
	}

	while ((ch = fgetc(source)) != EOF)
		fputc(ch, target);

	fclose(source);
	fclose(target);
	return 0;
}

void mkdir_recursive(const char *dir, int mode, bool onlyParents)
{
	int err = onlyParents ? 0 : mkdir(dir, mode);
	if (onlyParents || errno == ENOENT)
	{
		char *parentDir = malloc(strlen(dir) + 1);
		strcpy(parentDir, dir);

		char *str_found = NULL;
		if ((str_found = strstr_backward(parentDir, "/")) != NULL)
		{
			*str_found = 0;
			mkdir_recursive(parentDir, mode, false);
			free(parentDir);
		}

		if (!onlyParents)
			mkdir(dir, mode);
	}
}

int copy_file_or_directory(char *src, char *dest)
{
	printf("Copying '%s' to '%s'.\n", src, dest);

	struct stat statbuf;
	if (!stat(src, &statbuf))
	{
		if (S_ISDIR(statbuf.st_mode))
		{
			DIR *d = opendir(src);
			size_t src_path_len = strlen(src);
			size_t dst_path_len = strlen(dest);
			int r = -1;

			mkdir_recursive(dest, 0777, false);

			if (d)
			{
				struct dirent *p;

				r = 0;

				while (!r && (p = readdir(d)))
				{
					int r2 = -1;
					char *src_buf = NULL, *dst_buf = NULL;
					size_t src_len, dst_len;

					if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
						continue;

					src_len = src_path_len + strlen(p->d_name) + 2;
					dst_len = dst_path_len + strlen(p->d_name) + 2;

					src_buf = malloc(src_len);
					dst_buf = malloc(dst_len);

					if (src_buf && dst_buf)
					{
						snprintf(src_buf, src_len, "%s/%s", src, p->d_name);
						snprintf(dst_buf, dst_len, "%s/%s", dest, p->d_name);

						r2 = copy_file_or_directory(src_buf, dst_buf);
					}

					free(src_buf);
					free(dst_buf);

					r = r2;
				}

				closedir(d);
			}

			return r;
		}
		else
		{
			mkdir_recursive(dest, 0777, true);
			return copy_file(src, dest);
		}
	}
	return -1;
}

int copy_command(const char *sourceFileDir, const char *destPath)
{
	char soruceFileDirPath[512];
	strcpy(soruceFileDirPath, currentDirectory);

	if (soruceFileDirPath[0])
		strcat(soruceFileDirPath, "/");
	strcat(soruceFileDirPath, sourceFileDir);

	char destDirPath[512];
	strcpy(destDirPath, currentDirectory);

	if (destDirPath[0])
		strcat(destDirPath, "/");
	strcat(destDirPath, destPath);

	return copy_file_or_directory(soruceFileDirPath, destDirPath);
}

int move_command(const char *sourceFileDir, const char *destPath)
{
	if (!copy_command(sourceFileDir, destPath))
	{
		del_command(sourceFileDir);
		return 0;
	}
	return -1;
}

int rename_command(const char *sourcePath, const char *destPath)
{
	char sourceFullPath[512];
	char destFullPath[512];

	if (currentDirectory[0])
	{
		strcpy(sourceFullPath, currentDirectory);
		strcat(sourceFullPath, "/");
	}
	strcat(sourceFullPath, sourcePath);

	if (currentDirectory[0])
	{
		strcpy(destFullPath, currentDirectory);
		strcat(destFullPath, "/");
	}
	strcat(destFullPath, destPath);

	return rename(sourceFullPath, destFullPath);
}

void copy(char *cmd, char *arg)
{
	char *sourcePath = arg;
	char *destPath = NULL;
	if ((destPath = strstr(sourcePath, " ")) != NULL)
	{
		*destPath = 0;
		if (copy_command(sourcePath, destPath + 1))
			printf("Failed to run command.\n");
		else
			list_command();
	}
	else
	{
		printf("Error : %s : Invalid command.\n", cmd);
	}
}

void move(char *cmd, char *arg)
{
	char *sourcePath = arg;
	char *destPath = NULL;
	if ((destPath = strstr(sourcePath, " ")) != NULL)
	{
		*destPath = 0;
		if (move_command(sourcePath, destPath + 1))
			printf("Failed to run command.\n");
		else
			list_command();
	}
	else
		printf("Error : %s : Invalid command.\n", cmd);
}

int main()
{
	getcwd(currentDirectory, 512);

	printf("Simple Mobile C shell command line interpreter.\n");

	PrintHelp();

#ifdef __CCR__
	while (ccrGetEvent(true) != CCR_EVENT_QUIT)
#else
	while (true)
#endif
	{
		printf("example_shell> ");
		char buf[4096];
		memset(buf, 0, sizeof(buf));
		fgets(buf, sizeof(buf), stdin);
		Chomp(buf);
		if (!strcmp(buf, "help"))
			PrintHelp();
		else if (!strcmp(buf, "list") || !strcmp(buf, "ls"))
		{
			list_command();
		}
		else if (!strcmp(buf, "root") || !strcmp(buf, "cd /"))
		{
			root_command();
			list_command();
		}
		else if (!strcmp(buf, "home") || !strcmp(buf, "cd ~"))
		{
			home_command();
			list_command();
		}
		else if (!strcmp(buf, "up") || !strcmp(buf, "cd .."))
		{
			up_command();
			list_command();
		}
		else if (buf[0] == 'd' && buf[1] == 'o' && buf[2] == 'w' && buf[3] == 'n' && buf[4] == ' ')
		{
			down_command(buf + 5);
			list_command();
		}
		else if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ')
		{
			down_command(buf + 3);
			list_command();
		}
		else if (!strcmp(buf, "clear") || !strcmp(buf, "cls"))
		{
			clear_command();
		}
		else if (buf[0] == 'f' && buf[1] == 'i' && buf[2] == 'l' && buf[3] == 'e' && buf[4] == ' ')
		{
			file_command(buf + 5);
			list_command();
		}
		else if (buf[0] == 'd' && buf[1] == 'i' && buf[2] == 'r' && buf[3] == ' ')
		{
			dir_command(buf + 4);
			list_command();
		}
		else if (buf[0] == 'd' && buf[1] == 'e' && buf[2] == 'l' && buf[3] == ' ')
		{
			del_command(buf + 4);
			list_command();
		}
		else if (buf[0] == 'r' && buf[1] == 'm' && buf[2] == ' ')
		{
			del_command(buf + 3);
			list_command();
		}
		else if (buf[0] == 'c' && buf[1] == 'o' && buf[2] == 'p' && buf[3] == 'y' && buf[4] == ' ')
		{
			copy(buf, buf + 5);
		}
		else if (buf[0] == 'c' && buf[1] == 'p' && buf[2] == ' ')
		{
			copy(buf, buf + 3);
		}
		else if (buf[0] == 'm' && buf[1] == 'o' && buf[2] == 'v' && buf[3] == 'e' && buf[4] == ' ')
		{
			move(buf, buf + 5);
		}
		else if (buf[0] == 'm' && buf[1] == 'v' && buf[2] == ' ')
		{
			move(buf, buf + 3);
		}
		else if (buf[0] == 'r' && buf[1] == 'e' && buf[2] == 'n' && buf[3] == 'a' && buf[4] == 'm' && buf[5] == 'e' && buf[6] == ' ')
		{
			char *sourcePath = buf + 7;
			char *destPath = NULL;
			if ((destPath = strstr(sourcePath, " ")) != NULL)
			{
				*destPath = 0;
				if (rename_command(sourcePath, destPath + 1))
					printf("Failed to run command.\n");
				else
					list_command();
			}
			else
				printf("Error : %s : Invalid command.\n", buf);
		}
		else if (buf[0])
			printf("Error : %s : Invalid command.\n", buf);
	}
}

#else

#error "Not supported platform!"

#endif