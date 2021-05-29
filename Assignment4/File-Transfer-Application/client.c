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

#define MAX_CHAR_LEN 1024

void clientHandler(int clientSocket)
{
	char input[MAX_CHAR_LEN];
	char data[MAX_CHAR_LEN];

	while(1){
		scanf("%s", input);
		send(clientSocket,input, MAX_CHAR_LEN,0);

		int read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
		data[read] = '\0';
		printf("%s\n\n",data);

		if (strcmp(input, "exit") == 0) return;

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