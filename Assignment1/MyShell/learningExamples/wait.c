#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    // Example 3
    int status;
    // child process
    if (fork() == 0) exit(25);
    else wait(&status);
    printf("status = %d\n", status);
    if (WIFEXITED(status)) printf("Child exited normally with the exit status = %d\n", WEXITSTATUS(status));
    else if (WIFSIGNALED(status))

    // Example 2
    /* if (fork() == 0) printf("HC: hello from child! pid = %d\n", getpid());
    else 
    {
        printf("HP: Hello from parent! pid = %d\n", getpid());
        int cpid = wait(NULL);
        printf("CT: Child has terminated! wait() return value = %d\n", cpid);
    }
    printf("Bye\n"); */

    // Example 1
    /* pid_t cpid;
    if (fork() == 0) exit(0);
    else cpid = wait(NULL);
    printf("Parent pid = %d\n", getpid());
    printf("Child pid = %d\n", cpid); */

    return 0;
}