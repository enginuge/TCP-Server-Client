#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "5555"
#define MAXSIZE 512
int main()
{
	WSADATA wsadata;

	int sockfd, newfd;

	struct addrinfo hints, *serverinfo, *p;

	struct sockaddr_storage their_addr;

	socklen_t sin_size;

	char message[MAXSIZE] = "Robot Club Server is happy to meet you!";
	int rv;

	char receive_buffer[MAXSIZE];

	rv = WSAStartup(MAKEWORD(2,2), &wsadata);

	if(rv != 0)
	{
		printf("WSAStartup Failed: %d\n", rv);

		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

	rv = getaddrinfo(NULL, PORT, &hints, &serverinfo);

	if(rv  != 0)
	{
		fprintf(stderr, "getaddrinfo");
		return 1;
	}

	for(p = serverinfo; p != NULL; p= p -> ai_next)
	{
		sockfd = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol);

		if(sockfd == -1)
		{
			perror("server: socket");

			continue;
		}

		rv = bind(sockfd, p -> ai_addr, p-> ai_addrlen);

		if(rv == -1)
		{
			closesocket(sockfd);

			perror("Server: bind\n");

			continue;
		}

		break;
	}

	freeaddrinfo(serverinfo);

	if(p == NULL)
	{
		fprintf(stderr, "Server: failed to bind\n");

		exit(1);
	}

	rv = listen(sockfd, 10);

	if(rv == -1)
	{
		perror("Listen");

		exit(1);
	}


	printf("Server Waiting for Connection...\n");

	while(1)
	{
		sin_size = sizeof their_addr;

		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		
		printf("After Accept.\n");

		if(newfd == -1)
		{
			perror("accept");

			continue;
		}

		printf("Server Got Connection\n");

		rv = recv(newfd, receive_buffer, MAXSIZE-1, 0);

		if(rv == -1)
		{
			perror("recv");

			exit(1);
		}
		
		receive_buffer[rv] = '\0';

		printf(receive_buffer);
		printf("\n"); // newline in terminal.

		rv = send(newfd, message, strlen(message), 0);

		if(rv == -1)
		{
			perror("Send");
		}

		closesocket(newfd);

	}

	return 0;
}
