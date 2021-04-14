#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT "5555"
#define MAXSIZE 512

#define MAX_THREADS 10

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

DWORD WINAPI rec_worker()
{
	char rec_msg[MAXSIZE];	

	int rec_length=0;

	while(1)
	{
		rec_length = recv(newfd, rec_msg, MAXSIZE-1, 0);

		if(rec_length == -1)
		{
			perror("worker recv");

			exit(1);
		}
		
		rec_msg[rec_length] = '\0';

		// Display the recieved message.
		printf("Them: %s", rec_msg);
	}

	return 0;
}

DWORD WINAPI send_worker()
{
	char send_msg[MAXSIZE];

	int send_length = 0;

	while(1)
	{
		// Receive Client Intro.
		printf("You: ");
		fgets(send_msg, MAXSIZE, stdin);

		// Send Server Message.
		send_length = send(newfd, send_msg, strlen(send_msg), 0);

		if(send_length == -1)
		{
			perror("Send");

			exit(1);
		}
	}

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

	DWORD dwThreadIdArray[MAX_THREADS];

	HANDLE hThreadArray[MAX_THREADS];

	int threads_open = 0; // Count the open threads.

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

	hThreadArray[0] = CreateThread(NULL, 0, rec_worker, NULL, 0, &dwThreadIdArray[0]);
	threads_open++;
	hThreadArray[1] = CreateThread(NULL, 0, send_worker, NULL, 0, &dwThreadIdArray[1]);
	threads_open++;

	WaitForMultipleObjects(threads_open, hThreadArray, TRUE, INFINITE);
	
	closesocket(newfd);

	return 0;
}
