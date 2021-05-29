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


int main()
{
    char input[1024];
    scanf("%s", input);
    printf("%s\n", input);
    scanf("%s", input);
    printf("%s\n", input);

    return 0;
}