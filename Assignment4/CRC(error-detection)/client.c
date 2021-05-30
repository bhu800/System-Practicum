#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>

#define MAXD 82
#define MAXIN 200
#define MAXOUT 200

void getRemainder(char divisor[],char data[],int N,int dataLen,char remainder[]){
	char dividend[N+1];dividend[N]='\0';

	// sub dividend
	for(int i=0;i<N;i++)
	dividend[i]=data[i];

	// divisor,dividend -> remainder
	if(dividend[0]=='1'){
		for(int j=0;j<N;j++){
		  if(dividend[j]-divisor[j])
		    remainder[j]='1';
		  else
		    remainder[j]='0';
		}
	}
	else{
		for(int j=0;j<N;j++)
		  remainder[j]=dividend[j];
	}


	for(int i=N;i<dataLen;i++){
		// sub dividend
		for(int j=1;j<N;j++)
			dividend[j-1]=remainder[j];
		dividend[N-1]=data[i];

		// divisor,dividend -> remainder
		if(dividend[0]=='1'){
		  for(int j=0;j<N;j++){
		    if(dividend[j]-divisor[j])
		      remainder[j]='1';
		    else
		      remainder[j]='0';
		  }
		}
		else{
		  for(int j=0;j<N;j++)
		    remainder[j]=dividend[j];
		}
	}

	return ;
}

void getreq(char *inbuf, int len) {

	// divisor
	char divisor[]="11001";
	int N=(int)(sizeof(divisor)/sizeof(divisor[0]))-1;

	// chunk length
	int chunkLength=8;

	// data/dividend
	char data[MAXD];
	printf("Enter binary string (max length: 80): ");
	fgets(data,MAXD,stdin);
	int dataLen=strlen(data) - 1;

	// padding to get multiple of chunkLength
	if(dataLen%chunkLength){
		int i=dataLen;
		while(i%chunkLength){
			data[i]='0';dataLen++;i++;
		}
	}
	data[dataLen]='\0';

	// append extra bits for CRC
	int modifiedDataLen=(chunkLength + N-1)*ceil(dataLen/(1.0*chunkLength));
	char* modifiedData=(char*)(malloc(sizeof(char)*(modifiedDataLen+1)));
	// printf("%d\n", modifiedDataLen);


	// apply CRC on all chunks
	memset(modifiedData,0,modifiedDataLen+1);
	for(int i=0,j=0;i<dataLen;i+=chunkLength,j+=chunkLength+N-1){
		for(int k=0;k<chunkLength;k++){
			modifiedData[j+k]=data[i+k];
		}
		for(int k=0;k<N-1;k++)
			modifiedData[j+chunkLength+k]='0';

		char remainder[N+1];remainder[N]='\0';
		getRemainder(divisor,modifiedData+j,N,chunkLength+N-1,remainder);
		for(int k=0;k<N-1;k++){
			modifiedData[j+chunkLength+k]=remainder[k+1];
		}
	}

	// printf("%s\n", modifiedData);


	memset(inbuf,0,len);
	for(int i=0;i<modifiedDataLen;i++)
	inbuf[i]=modifiedData[i];
	printf("%s\n", inbuf);
	free(modifiedData);

	return;

}

void client(int sockfd) {
  int n;
  char sndbuf[MAXIN]; char rcvbuf[MAXOUT];
  getreq(sndbuf, MAXIN);
  while (strlen(sndbuf) > 0) {
  	// send
    write(sockfd, sndbuf, strlen(sndbuf));
    // printf("Wrote message: %s\n",sndbuf);
    
    // receive
    memset(rcvbuf,0,MAXOUT);
    n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
    // printf("Received reply: %d",n);
    write(STDOUT_FILENO, rcvbuf, n);

    // again
    getreq(sndbuf, MAXIN);
  }
}

// server addr
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,char *serverIP, int portno) {
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main() {
	// client
	char *serverIP = "127.0.0.1";
	int sockfd, portno = 5033;
	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	// create socket with TCP
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// connect to server
	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("Connected to %s:%d\n",serverIP, portno);

	// work
	client(sockfd);

	// close conn
	close(sockfd);

	return 0;
}