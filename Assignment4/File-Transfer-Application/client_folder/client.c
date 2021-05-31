#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

// colors for using in output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define COLOR_RESET  "\033[0m"

#define MAX_CHAR_LEN 1024

void readLine(char* line)
{
    if (fgets(line, MAX_CHAR_LEN+1, stdin) == NULL)
    {
        if (feof(stdin))
        {
            printf(ANSI_COLOR_MAGENTA "Bye!\n" ANSI_COLOR_RESET);
            exit(0);
        }
        printf("command input: Too long input, out of buffer range.");
        return;
    }
}

void clientHandler(int clientSocket)
{
	char input[MAX_CHAR_LEN];
	char data[MAX_CHAR_LEN];
	int read;
	char* filename;

	while(1)
	{
		printf("filename 0 : %s\n", filename);
		readLine(input);
		input[strlen(input)-1] = '\0';
		send(clientSocket,input, MAX_CHAR_LEN,0);

		char* arg;
		char* rest = input;
		arg = strtok_r(rest, " ", &rest);

		if (strcmp(arg, "reqFile") == 0)
		{
			printf("hello!!\n");
			arg = strtok_r(rest, " ", &rest);
			filename = strdup(arg);
		}
		printf("filename 1 : %s\n", filename);

		read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
		data[read] = '\0';

		if (strcmp(data, "SEND_START") == 0)
		{
			printf("In send_start\n");
			read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
			data[read] = '\0';
			printf("%s\n\n", data);

			

			printf("filename - %s\n", filename);

			FILE *fp;
			// char *filename = arg;

			fp = fopen(filename, "w");

			if (fp == NULL) 
			{
				printf("ERROR detected!!\n");
			}

			while (1) 
			{
				printf("In fwrite while loop\n");
				read = recv(clientSocket, data, MAX_CHAR_LEN, 0);

				if (strcmp(data, "SEND_COMPLETE") == 0 || strcmp(data, "SEND_FAILED") == 0)
				{
					break;
				}

				int data_len = atoi(data);

				printf("data_len: %d\n", data_len);

				read = recv(clientSocket, data, MAX_CHAR_LEN, 0);

				fwrite(data, sizeof(char), data_len, fp);
				bzero(data, MAX_CHAR_LEN);
			}
			fclose(fp);
			read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
			data[read] = '\0';
			printf("%s\n\n",data);
		}
		else
		{
			printf("%s\n\n",data);
		}

		if (strcmp(input, "exit") == 0) 
		{
			free(filename);
			return;
		}

		bzero(input, MAX_CHAR_LEN);
	}

}

int main(){

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1)
    {
        printf("ERROR CREATING SOCKET\n");
        return 0;
    }

	// to be manually filled
	struct sockaddr_in serverAddr;
	int server_port = 8080;
	char server_ip[] = "127.0.0.1";

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8080); // server port
	// INADDR_ANY - binds the socket to all available interfaces
    // to bind your socket to localhost only, the syntax would be -
    // my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // server ip

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) {
		printf("ERROR CONNECTING WITH SERVER\n");
		return 0;
	}

	printf("Connection established with server %s at port %d ............\n", server_ip, server_port);

	clientHandler(clientSocket);

	if (close(clientSocket) == -1)
	{
		perror("ERROR - failed to close socket");
	}
	else printf("Closed Socket successfully\nBye!\n");

	return 0;

}