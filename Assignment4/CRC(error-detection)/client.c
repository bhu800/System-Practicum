#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXD 81
#define MAXIN 100
#define MAXOUT 100

void getRemainder(char divisor[],char data[],int N,int dataLen,char remainder[]){
  char dividend[N+1];dividend[N]='\0';

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
  // printf("%s\n", dividend);
  // printf("%s\n\n", remainder);


  for(int i=N;i<dataLen;i++){
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

    // printf("%s\n", dividend);
    // printf("%s\n\n", remainder);
  }

  return ;
}

void getreq(char *inbuf, int len) {

  // divisor
  char divisor[]="11001";
  int N=(int)(sizeof(divisor)/sizeof(divisor[0])) - 1;

  // data/dividend
  char data[MAXD];
  printf("Enter binary string (max length: 80): ");
  fgets(data,MAXD,stdin);
  int dataLen=strlen(data) - 1;
  data[dataLen]='\0';

  // append extra bits for CRC
  int modifiedDataLen=dataLen + N-1;
  char* modifiedData=(char*)(malloc(sizeof(char)*(modifiedDataLen+1)));

  for(int i=0;i<dataLen;i++)
    modifiedData[i]=data[i];
  for(int i=dataLen;i<modifiedDataLen;i++)
    modifiedData[i]='0';
  modifiedData[modifiedDataLen]='\0';

  // get CRC/Remainder, modify CRC bits
  char remainder[N+1];remainder[N]='\0';
  getRemainder(divisor,modifiedData,N,modifiedDataLen,remainder);
  for(int i=dataLen;i<modifiedDataLen;i++)
    modifiedData[i]=remainder[i-dataLen+1];

  // printf("%s\n", remainder);
  // printf("%s\n",modifiedData);

  memset(inbuf,0,len);
  for(int i=0;i<MAXD;i++)
    inbuf[i]=modifiedData[i];

  free(modifiedData);

  return;

}

void client(int sockfd) {
  int n;
  char sndbuf[MAXIN]; char rcvbuf[MAXOUT];
  getreq(sndbuf, MAXIN);
  while (strlen(sndbuf) > 0) {
    write(sockfd, sndbuf, strlen(sndbuf)); /* send */
    //printf("Wrote message: %s\n",sndbuf);
    
    memset(rcvbuf,0,MAXOUT);
    n=read(sockfd, rcvbuf, MAXOUT-1);      /* receive */
    // printf("Received reply: %d",n);
    write(STDOUT_FILENO, rcvbuf, n);

    getreq(sndbuf, MAXIN);
  }
}

// Server address
struct hostent *buildServerAddr(struct sockaddr_in *serv_addr,
	char *serverIP, int portno) {
  /* Construct an address for remote server */
  memset((char *) serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr->sin_family = AF_INET;
  inet_aton(serverIP, &(serv_addr->sin_addr));
  serv_addr->sin_port = htons(portno);
 }


int main() {
	//Client protocol
	char *serverIP = "127.0.0.1";
	int sockfd, portno = 5035;
	struct sockaddr_in serv_addr;
	
	buildServerAddr(&serv_addr, serverIP, portno);

	/* Create a TCP socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Connect to server on port */
	connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("Connected to %s:%d\n",serverIP, portno);

	/* Carry out Client-Server protocol */
	client(sockfd);

	/* Clean up on termination */
	close(sockfd);
}