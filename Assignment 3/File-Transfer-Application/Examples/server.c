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

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

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

const MAX_THREADS = 1024;
const MAX_CHAR_LEN = 1024;

struct client Client[MAX_THREADS];
pthread_t thread[MAX_THREADS];

void * doNetworking(void * ClientDetail){

	printf("Client %d connected.\n",index + 1);

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;

	clientDetail->usageDetails.numFilesDown = 0;
	clientDetail->usageDetails.numFilesReq = 0;
	clientDetail->usageDetails.sizeDataTransferred = 0;

	while(1){

		char data[MAX_CHAR_LEN];
		int read = recv(clientSocket, data, MAX_CHAR_LEN, 0);
		data[read] = '\0';

		char output[MAX_CHAR_LEN];

		if(strcmp(data, "usage-details") == 0)
		{

			int l = 0;

			sprintf(output, "*** Usage Details***\nNumber of files requested : %d\nNumer of files received : %d\nSize of data transferred : %d Bytes\n***********\n");

			send(clientSocket, output, strlen(output), 0);

		}
		else if(strcmp(data,"SEND") == 0){

			read = recv(clientSocket,data,MAX_CHAR_LEN,0);
			data[read] = '\0';

			int id = atoi(data) - 1;

			read = recv(clientSocket,data,MAX_CHAR_LEN,0);
			data[read] = '\0';

			send(Client[id].sockID,data,MAX_CHAR_LEN,0);			

		}

	}

	close(clientSocket);

	clientDetail->isOccupied = false;

	return NULL;
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
    unsigned short port = 8080;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
    // INADDR_ANY - binds the socket to all available interfaces
    // to bind your socket to localhost only, the syntax would be -
    // my_sockaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY); 

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

	printf("Server started listenting on port %hu ...\n", port);

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

	return 0;
}