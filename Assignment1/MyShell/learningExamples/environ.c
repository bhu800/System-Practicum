#include <stdio.h>

extern char **environ;

int main(int argc, char** argv, char **envp)
{
    char **env = envp;
    while (*env != NULL)
    {
        printf("%s\n\n", *env);
        env++;
    }
    printf("\n\n-----------------------------\n\n");
    env = environ;
    while (*env != NULL)
    {
        printf("%s\n\n", *env);
        env++;
    }


    return 0;
}