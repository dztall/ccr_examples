//Authors : Lee Jeong Seop
//CC BY-NC-SA 4.0

#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support native to script callback function yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include <regex>

void exit_handler();

#define SERVER_PORT 8254

struct User
{
	int socket;
};

int sockfd = -1;
std::list<User> users;
std::list<std::string> recentMessages;
std::mutex mutex;

pthread_t thread;
bool exitThread = false;

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

void EnqueueMessage(const std::string &msg)
{
	if (recentMessages.size() > 100)
		recentMessages.erase(recentMessages.begin());
	recentMessages.push_back(msg);
}

bool is_filtered_message(const std::string &message)
{
	std::string beginsWith = "GET ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "POST ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "HEAD ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "PUT ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "DELETE ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "TRACE ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "OPTIONS ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	beginsWith = "CONNECT ";
	if (message.compare(0, beginsWith.length(), beginsWith) == 0)
		return true;
	return false;
}

void *server_thread(void *param)
{
	while (!exitThread)
	{
		usleep(1000 * 10);

		mutex.lock();

		std::vector<int> removeList;
		for (const auto &user : users)
		{
			std::string message;
			char buf[1024];
			ssize_t n = recv(user.socket, buf, 1023, 0);
			if (n < 0)
			{
				int err = errno;
				if (err != EWOULDBLOCK && err != EINTR)
					removeList.push_back(user.socket);
				continue;
			}
			else if (n == 0)
			{
				removeList.push_back(user.socket);
				continue;
			}
			buf[n] = 0;
			message += buf;
			while (true)
			{
				ssize_t n = recv(user.socket, buf, 1023, 0);
				if (n < 0)
					break;
				else if(n == 0)
				{
					removeList.push_back(user.socket);
					break;
				}
				buf[n] = 0;
				message += buf;
			}

			if (is_filtered_message(message))
				continue;

			std::string outputMessage = std::to_string(user.socket) + " : " + message;
			printf("%s", outputMessage.c_str());
			EnqueueMessage(outputMessage);

			for (const auto &user1 : users)
			{
				if (user1.socket == user.socket)
					continue;
				ssize_t sentSize = send(user1.socket, outputMessage.c_str(), outputMessage.length() + 1, 0);
				if (sentSize <= 0)
					removeList.push_back(user.socket);
			}
		}
		for (int removeItem : removeList)
		{
			users.erase(std::remove_if(users.begin(), users.end(), [=](const User &user)
			{
				return user.socket == removeItem;
			}));
			close(removeItem);
		}
		for (int removeItem : removeList)
		{
			std::string exitUserMessage = "User " + std::to_string(removeItem) + " exits the room.\n";
			printf("%s", exitUserMessage.c_str());
			for (const auto &user : users)
				send(user.socket, exitUserMessage.c_str(), exitUserMessage.length() + 1, 0);
		}

		mutex.unlock();
	}

	return nullptr;
}

//Copied from : https://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
void showIP()
{
	struct ifaddrs *ifaddr, *ifa;
	int s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1)
	{
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	printf("List of ip addresses :\n");

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL)
			continue;

		s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

		if (ifa->ifa_addr->sa_family == AF_INET)
		{
			if (s != 0)
			{
				printf("getnameinfo() failed: %s\n", gai_strerror(s));
				exit(EXIT_FAILURE);
			}
			printf("\tInterface : <%s>\n", ifa->ifa_name);
			printf("\t  Address : <%s>\n", host);
		}
	}

	freeifaddrs(ifaddr);
}

int main(int argc, char *argv[])
{
	//Clang/LLVM on iOS does not support function pointer yet.
#if !CCR_FORCE_LLVM_INTERPRETER
	atexit(exit_handler);
#endif

	struct sockaddr_in serv_addr, cli_addr;

	showIP();

	signal(SIGPIPE, SIG_IGN);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
		error("Can't open socket.");

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	memset((char *)&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Socket binding error.");

	listen(sockfd, 5);

	pthread_create(&thread, NULL, server_thread, nullptr);

	printf("Chatting server waiting for clients...\n");

	while (1)
	{
		socklen_t clilen = sizeof(cli_addr);
		int clientSocket = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

		if (clientSocket < 0)
			error("Socket accepting error.");

		int flags = fcntl(clientSocket, F_GETFL, 0);
		fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK);

		//Disable nagle's algorithm.
		int flag = 1;
		setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));

		mutex.lock();

		std::string welcomeMessage = "Hello, This is example chatting server.\n";
		welcomeMessage += "Your chatting ID is " + std::to_string(clientSocket) + ".\n\n";

		//Send recent messages to newcomer.
		for (const auto &recentMessage : recentMessages)
			welcomeMessage += recentMessage;

		send(clientSocket, welcomeMessage.c_str(), welcomeMessage.length() + 1, 0);

		std::string newUserMessage = "Here comes new user! Chatting ID is " + std::to_string(clientSocket) + ".\n";
		printf("%s", newUserMessage.c_str());
		for (const auto &user : users)
			send(user.socket, newUserMessage.c_str(), newUserMessage.length() + 1, 0);
		users.push_back({ clientSocket });

		mutex.unlock();
	}

#if CCR_FORCE_LLVM_INTERPRETER
	exit_handler();
#endif

	return 0;
}

//Called on normal/abnormal exit.
void exit_handler()
{
	exitThread = true;
	pthread_join(thread, nullptr);
	
	for (const auto &user : users)
	{
		if(user.socket >= 0)
			close(user.socket);
	}
	users.clear();

	if(sockfd >= 0)
	{
		close(sockfd);
		sockfd = -1;
	}
}
