//Original source code found at : http://www.cplusplus.com/forum/windows/17264/

#if !defined(_WIN32)
#error "Not supported platform!"
#endif

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

int main()
{
	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("%s", "WSAStartup failed.\n");
		return 1;
	}

	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct hostent *host;
	host = gethostbyname("www.example.com");

	SOCKADDR_IN SockAddr;
	SockAddr.sin_port = htons(80);
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

	connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr));
	send(Socket, "GET  HTTP/1.0\r\n\r\n", strlen("GET  HTTP/1.0\r\n\r\n"), 0);
	char buffer[10000];

	int nDataLength = recv(Socket, buffer, sizeof(buffer), 0);
	printf("%s", buffer);

	closesocket(Socket);
	WSACleanup();

	return 0;
}