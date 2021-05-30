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
#include <stdbool.h>
#include <string.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define MAX_THREADS 1024
#define MAX_CHAR_LEN 1024

struct usageDetails
{
	int numFilesReq;
	int numFilesDown;
	int sizeDataTransferred;
};

struct client{

	int index;
	int sockID;
	struct sockaddr_in clientAddr;
	int len;
	bool isOccupied;
	struct usageDetails usageDetails;
};

struct client Client[MAX_THREADS];
pthread_t thread[MAX_THREADS];

void * doNetworking(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	printf("Client %d connected.\n", index + 1);

	clientDetail->usageDetails.numFilesDown = 0;
	clientDetail->usageDetails.numFilesReq = 0;
	clientDetail->usageDetails.sizeDataTransferred = 0;

	char data[MAX_CHAR_LEN];
	char response[MAX_CHAR_LEN];

	while(1)
	{
		bzero(data, MAX_CHAR_LEN);
		int read = recv(clientSocket, data, MAX_CHAR_LEN, 0);

		if (read == 0)
		{
			sprintf(response, "Bye!");
			send(clientSocket, response, strlen(response), 0);

			close(clientSocket);

			clientDetail->isOccupied = false;

			printf("client socked %d closed\n", clientSocket);

			return NULL;
		}

		printf("DEBUG: read - %d\n", read);
		data[read] = '\0';

		printf("DATA - %s\n", data);

		char* arg;
		char* rest = data;
		arg = strtok_r(rest, " ", &rest);

		printf("arg 1: %s\n", arg);

		if(strcmp(arg, "usage-details") == 0)
		{

			sprintf(response, "*** Usage Details***\nNumber of files requested : %d\nNumer of files received : %d\nSize of data transferred : %d Bytes\n***********",\
			clientDetail->usageDetails.numFilesDown, clientDetail->usageDetails.numFilesReq, clientDetail->usageDetails.sizeDataTransferred);

			send(clientSocket, response, strlen(response), 0);

		}
		// reqFile <filename>
		else if(strcmp(arg, "reqFile") == 0)
		{
			arg = strtok_r(rest, " ", &rest); // filename

			printf("arg 2: %s\n", arg);
			
			bool file_found = false;
			/*
			check here if file is present at the server 
			*/
			if (file_found)
			{
				// confirm if the user wants to download the file
				sprintf(response, "Requested file %s is available. Do you want to downlaod the file? [Y/N] ", arg);
				send(clientSocket, response, strlen(response), 0);

				// read the user's response
				read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
				data[read] = '\0';

				while (1)
				{
					if (strcmp(data, "Y") || strcmp(data, "y") || strcmp(data, "Yes") || strcmp(data, "yes"))
					{
						// sending message to the user
						sprintf(response, "We are sending the file %s\n", arg);
						send(clientSocket, response, strlen(response), 0);

						/*
						start sending the data 
						also handle - file transfer succeful and failure
						*/
						break;
					}
					else if (strcmp(data, "N") || strcmp(data, "n") || strcmp(data, "No") || strcmp(data, "no"))
					{
						break;
					}
					else
					{
						sprintf(response, "Please choose Y (for yes) or N (for no) ");
						send(clientSocket, response, strlen(response), 0);
					}
				}
			}
			else
			{
				// File not available message
				sprintf(response, "Sorry the requested file %s is NOT AVAILABLE on the server", arg);
				send(clientSocket, response, strlen(response), 0);
			}
		}
		else if (strcmp(arg, "exit") == 0)
		{
			sprintf(response, "Bye!");
			send(clientSocket, response, strlen(response), 0);

			close(clientSocket);

			clientDetail->isOccupied = false;

			printf("client socked %d closed\n", clientSocket);

			return NULL;
		}
		else if (strcmp(arg, "help") == 0)
		{
			sprintf(response, "COMMANDS : \nusage-details : usage details (list of files client has downloaded so far, size of data transferred etc)\nreqFile <filename> : request file <filename> to be downloaded from the server\nexit : exit the application");
			send(clientSocket, response, strlen(response), 0);
		}
		else
		{
			sprintf(response, "ERROR: no such command");
			send(clientSocket, response, strlen(response), 0);
		}

	}

	// dose not handle - 
	// if client closes without sending exit message
	// possible ways to handle - timeout or try writing some message to client after a fixed time
}

int main(){

	int currThreadIdx = 0;
	for (int i=0; i<MAX_THREADS; i++) Client[i].isOccupied = false;

    // setup socket
	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        printf("ERROR CREATING SOCKET\n");
        return 0;
    }

	struct sockaddr_in serverAddr;
    unsigned short server_port = 8080; // <-- server port
	char server_ip[] = "127.0.0.1"; // <-- put here server machine's ip

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(server_port);
    // INADDR_ANY - binds the socket to all available interfaces
    // to bind your socket to localhost only, the syntax would be -
    // my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);  // use server_ip here

    // bind the server socket to given ip and port
	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1)
    {
        printf("ERROR BINDING SOCKET\n");
        return 0;
    }

    // listen for incoming connections
	if(listen(serverSocket,1024) == -1)
    {
        printf("ERROR LISTENING CONNECTIONS\n");
        return 0;
    }

	printf("Server started listenting on port %hu ...\n", server_port);

    // accept client connections, put them in array and start execution in a new thread
	while(1){

		for (int i=0; i<MAX_THREADS; i++) 
		{
			currThreadIdx = (currThreadIdx + i)%MAX_THREADS;
			if (!Client[currThreadIdx].isOccupied) break; 
		}

		// check if any thread is available currently
		if (Client[currThreadIdx].isOccupied)
		{
			printf("SERVER CAPACITY IS FULL CURRENTLY. PLEASE TRY LATER\n");
		}
		else
		{
			Client[currThreadIdx].sockID = accept(serverSocket, (struct sockaddr*) &Client[currThreadIdx].clientAddr, &Client[currThreadIdx].len);
			Client[currThreadIdx].index = currThreadIdx;
			Client[currThreadIdx].isOccupied = true;

			pthread_create(&thread[currThreadIdx], NULL, doNetworking, (void *) &Client[currThreadIdx]);

			currThreadIdx++;
		}
	}

    // main thread waits for client threads top complete
	for(int i = 0 ; i < MAX_THREADS ; i ++)
		if (Client[i].isOccupied) pthread_join(thread[i],NULL);

	close(serverSocket);

	printf("server closed. Bye!\n");

	return 0;
}