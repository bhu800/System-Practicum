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


int main()
{
    char input[1024];
    readLine(input);
    input[strlen(input)-1] = '\0';
    // scanf("%s", input);
    // printf("%s\n", input);
    
    char* arg;
    char* rest = input;
    arg = strtok_r(rest, " ", &rest);
    printf("%d\n", strlen(arg));
    if (strcmp(arg, "help") == 0) printf("Identified !!\n");

    // arg = strtok_r(rest, " ", &rest);
    // printf("filename - %s\n", arg);

    return 0;
}