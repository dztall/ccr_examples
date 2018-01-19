//Original source code found at : http://tenouk.com/cpluscodesnippet/createathread.html

#ifndef _WIN32
#error "Not supported platform!"
#endif

#include <stdio.h>
#include <conio.h>

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	printf("The parameter: %u.\n", *(DWORD*)lpParam);
	return 0;
}

int main(void)
{
	DWORD dwThreadId, dwThrdParam = 1;
	HANDLE hThread;

	hThread = CreateThread(
		NULL, // default security attributes
		0, // use default stack size 
		MyThreadFunction, // thread function
		&dwThrdParam, // argument to thread function
		0, // use default creation flags
		&dwThreadId); // returns the thread identifier

	printf("The thread ID: %d.\n", dwThreadId);

	// Check the return value for success. If something wrong...
	if (hThread == NULL)
		printf("CreateThread() failed, error: %d.\n", GetLastError());
	//else, gives some prompt...
	else
		printf("It seems the CreateThread() is OK lol!\n");

	if (CloseHandle(hThread) != 0)
		printf("Handle to thread closed successfully.\n");

	return 0;
}
