#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "5555"
#define MAXSIZE 512

int write_file(int sockfd, char filename[])
{
	int n;
	int total_n = 0;

	FILE *fp;
	char buffer[MAXSIZE];

	fp = fopen(filename, "wb");

	while(1)
	{
		n = recv(sockfd, buffer, MAXSIZE, 0);
		
		printf("SERVER: Received %d Bytes.\n", n);

		if(n<= 0)
		{
			break;
		}

		//fprintf(fp, "%s", buffer);
		fwrite(buffer, sizeof(char), n, fp);
		
		total_n += n;

		memset(buffer, 0, MAXSIZE);
	}

	printf("SERVER: Total Bytes Received: %d\n", total_n);
	
	fclose(fp);

	return 0;
}

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

	sin_size = sizeof their_addr;

	newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	
	printf("After Accept.\n");

	if(newfd == -1)
	{
		perror("accept");

		//continue;
		return -1;
	}

	printf("Server Got Connection!\n");

	while(1)
	{

		// Receive Client Intro.
		rv = recv(newfd, receive_buffer, MAXSIZE-1, 0);

		if(rv == -1)
		{
			perror("recv");

			exit(1);
		}
		
		receive_buffer[rv] = '\0';

		// Display the recieved message.
		printf("Them: %s", receive_buffer);
		//printf("\n"); // newline in terminal.

		printf("You: ");
		fgets(message, MAXSIZE, stdin);

		// Send Server Message.
		rv = send(newfd, message, strlen(message), 0);

		if(rv == -1)
		{
			perror("Send");
		}
	}

	closesocket(newfd);

	return 0;
}
