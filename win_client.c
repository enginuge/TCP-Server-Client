#include <stdio.h>
#include <stdlib.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "5555"
#define MAXSIZE 512

int main(int argc, char *argv[])
{
	WSADATA wsadata;

	int sockfd, numbytes;

	char buf[MAXSIZE];

	struct addrinfo hints, *serverinfo, *p;

	int rv;

	if(argc!=2)
	{
		fprintf(stderr, "Program takes IP input argument\n");

		exit(1);
	}

	rv = WSAStartup(MAKEWORD(2,2), &wsadata);

	if(rv != 0)
	{
		printf("WSAStartup Failed: %d\n", rv);

		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	rv = getaddrinfo(argv[1], PORT, &hints, &serverinfo);

	if(rv != 0)
	{
		fprintf(stderr, "Getaddrinfo");

		return 1;
	}

	for(p = serverinfo; p != NULL; p -> ai_next)
	{
		sockfd = socket(p -> ai_family, p -> ai_addrlen, p -> ai_protocol);

		if(sockfd == -1)
		{
			closesocket(sockfd);

			perror("client: connect");

			continue;
		}

		break;
	
	}

	if(p == NULL)
	{
		fprintf(stderr, "Client: Failed to Connect\n");

		exit(1);
	}

	freeaddrinfo(serverinfo);

	numbytes = recv(sockfd, buf, MAXSIZE-1, 0);

	if(numbytes == -1)
	{
		perror("recv");

		exit(1);
	}

	buf[numbytes] ='\0';

	printf("Client: Received --------\n%s\n---------\n", buf);

	closesocket(sockfd);

	return 0;
}

