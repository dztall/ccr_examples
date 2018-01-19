#if !defined(_OS_IOS_) && !defined(_OS_OSX_) && !defined(_OS_ANDROID_)
#error "Not supported platform!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

int main()
{
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[256];

	portno = 80;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("ERROR opening socket");
		return 0;
	}

	server = gethostbyname("example.com");
	if (server == NULL)
	{
		printf("ERROR, no such host\n");
		return 0;
	}

	memset((char *)&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);

	memcpy((char *)&serv_addr.sin_addr.s_addr,
		(char *)server->h_addr, server->h_length);

	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("ERROR connecting : %d", errno);
		return 0;
	}

	send(sockfd, "GET /\n", 6, MSG_DONTWAIT);

	memset(buffer, 0, 256);
	n = recv(sockfd, buffer, 255, 0);
	if (n < 0)
		printf("ERROR reading from socket");

	printf("%s\n", buffer);

	return 0;
}