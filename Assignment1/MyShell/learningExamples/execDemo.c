#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    // Example 1
    /* char* args[] = {"ls", "-l", "-a", NULL};
    int a = execvp(args[0], args);

    printf("Return value: %d\n*** Ending ***\n", a); */

    /* char* args[] = {"cd", "..", NULL};
    int a = execvp(args[0], args);

    printf("Return value: %d\n*** Ending ***\n", a); */

    char* args[] = {"echo", "hello", NULL};
    int a = execvp(args[0], args);

    printf("Return value: %d\n*** Ending ***\n", a);

    // Example 2
    /* char* args[] = {"ls -l", "-a", NULL};
    int a = execvp(args[0], args);

    printf("Return value: %d\n*** Ending ***\n", a); */

    // Example 3
    /* char* args[] = {"./EXEC", NULL};
    int a = execvp(args[0], args);

    printf("Return value: %d\n*** Ending ***\n", a); */

    return 0;
}