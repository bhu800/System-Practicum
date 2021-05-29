#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdbool.h>
#define MAXREQ 100
#define MAXQUEUE 5

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

void server(int consockfd) {

  // divisor
  char divisor[]="11001";
  int N=(int)(sizeof(divisor)/sizeof(divisor[0])) - 1;

  char reqbuf[MAXREQ];
  int n;
  while (1) {                   
    memset(reqbuf,0, MAXREQ);
    n = read(consockfd,reqbuf,MAXREQ-1); /* Recv */
    // printf("Received message: %s\n", reqbuf);
    if (n <= 0) return;

    int dataLen=strlen(reqbuf);
    char* data=(char*)(malloc(sizeof(char)*(dataLen+1)));
    for(int i=0;i<dataLen;i++)
      data[i]=reqbuf[i];

    char remainder[N+1];remainder[N]='\0';
    getRemainder(divisor,data,N,dataLen,remainder);
    // printf("%s\n", remainder);

    bool flag=false;  // isError
    for(int i=0;i<N;i++){
      if(remainder[i]!='0'){
        flag=true;break;
      }
    }

    if(flag){
      printf("** Error Detected **\n");
      
      memset(reqbuf,0,strlen(reqbuf));
      char message[]="Error in transmission\n";
      for(int i=0;i<strlen(message);i++)
        reqbuf[i]=message[i];
    }
    else{
      printf("** No Error **\n");

      memset(reqbuf,0,strlen(reqbuf));
      char message[]="Successful transmission\n";
      for(int i=0;i<strlen(message);i++)
        reqbuf[i]=message[i];
    }

    
    write(consockfd, reqbuf, strlen(reqbuf));

    free(data);
  }
}

int main() {

int lstnsockfd, consockfd, clilen, portno = 5035;
struct sockaddr_in serv_addr, cli_addr;

 memset((char *) &serv_addr,0, sizeof(serv_addr));
 serv_addr.sin_family      = AF_INET;
 serv_addr.sin_addr.s_addr = INADDR_ANY;
 serv_addr.sin_port        = htons(portno);

// Server protocol
/* Create Socket to receive requests*/
lstnsockfd = socket(AF_INET, SOCK_STREAM, 0);

/* Bind socket to port */
bind(lstnsockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr));
printf("Bounded to port\n");
while (1) {
   printf("Listening for incoming connections\n");

/* Listen for incoming connections */
   listen(lstnsockfd, MAXQUEUE); 

   //clilen = sizeof(cl_addr);

/* Accept incoming connection, obtaining a new socket for it */
   consockfd = accept(lstnsockfd, (struct sockaddr *) &cli_addr,       
                      &clilen);
   printf("Accepted connection\n");

   server(consockfd);

   close(consockfd);
  }
close(lstnsockfd);
}
