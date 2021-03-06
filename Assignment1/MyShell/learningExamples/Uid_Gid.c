#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

const char *getUserName()
{
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw) return pw->pw_name;
  return "";
}

int main()
{
    


    /* printf("%s\n", getUserName()); */
    
    /* int val;

    printf("This process is %d\n", getpid());
    printf("The real user ID is %d\n", getuid());
    printf("The effective user ID is %d\n", geteuid());
    printf("Real group ID: %d\n", getgid());
    printf("Effective group ID: %d\n", getegid());

    if ((val = fork()) < 0)
    {
        printf("Problem creating new process\n");
        return 1;
    }
    if (val == 0)
    {
        printf("This process is %d\n", getpid());
        printf("The real user ID is %d\n", getuid());
        printf("The effective user ID is %d\n", geteuid());
        printf("Real group ID: %d\n", getgid());
        printf("Effective group ID: %d\n", getegid());
    } */

    return 0;
}