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
    char input[] = "reqFile Bhumi";
    char filename[1024];

    char* arg;
    char* rest = input;
    arg = strtok_r(rest, " ", &rest);
    printf("arg1 : %s\n", arg);
    if (strcmp(arg, "reqFile") == 0)
    {
        arg = strtok_r(rest, " ", &rest);
        printf("arg2 : %s\n", arg);
        strcpy(filename, arg);
    }

    printf("%s\n", filename);

    sprintf(input, "hello world");
    rest = input;
    arg = strtok_r(rest, " ", &rest);

    printf("%s\n", filename);

    return 0;
}