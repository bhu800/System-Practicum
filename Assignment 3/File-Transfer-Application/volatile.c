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
    for (int i=0; i<5; i++)
    {
        printf("@@ Loop 1: %d\n", i);
        for (int j=0; j<5; j++)
        {
            printf("## Loop 2: %d\n", j);
            if (j==1) break;
        }
    }

    return 0;
}