#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

// Handler for SIGINT
// caused by Ctrl+C at keyboard
void handle_sigint(int sig)
{
    printf("I just handled SIGINT signal. It's COOL!\n");
    exit(1);
}

int main()
{
    signal(SIGINT, handle_sigint);
    while(1)
    {
        printf("Hello\n");
        sleep(1);
    }

    return 0;
}