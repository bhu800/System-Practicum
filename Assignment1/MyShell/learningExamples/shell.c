#include <stdio.h>
#include <sys/types.h>

void parse(char* line, char **argv)
{
    while (line[0] != '\0') // i.e. if not the end of the line
    {
        // replace white spaces with \0
        while (line[0] == ' ' || line[0] == '\t' || line[0] == '\n') *line++ = '\0';
        // save the argument position
        *argv++ = line;
        // skip the arguments until - 
        while (*line != '\0' && *line != ' ' && *line != '\t' && *line != '\n') line++;
    }
    *argv = '\0';
}

void execute(char **argv)
{
    pid_t pid;
    int status;
    // fork a child process
    if ((pid = fork()) < 0)
    {
        printf("ERROR: forking child process failed.\n");
        exit(1);
    }
    // for child process - 
    else if (pid == 0)
    {
        if (execvp(*argv, argv) < 0)
        {
            printf("ERROR: Execution failed\n");
            exit(1);
        }
    }
    // for parent process - 
    else
    {
        while (wait(&status) != pid);
    }
}

void main(void)
{
    char line[1024];
    char* argv[64];

    while (1)
    {
        printf("SHELL > ");
        gets(line);
        printf("\n");
        parse(line, argv);
        if (strcmp(argv[0], "exit") == 0) exit(0);
        execute(argv);
    }
}