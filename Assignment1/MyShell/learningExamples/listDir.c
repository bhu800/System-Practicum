/**
 * C program to list file and sub-directories of a directory 
 * recursively in tree structure.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


void tree(char *basePath, const int root);

int main()
{
    // Directory path to list files
    char path[100];

    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);

    tree(path, 0);

    return 0;
}


/**
 * Tree, prints all files and sub-directories of a given 
 * directory in tree structure.
 * 
 * @param basePath Base path to traverse directory
 * @param root     Integer representing indention for current directory
 */
void tree(char *basePath, const int root)
{
    int i;
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
    {
        // perror("SHELL: dir: ");
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            for (i=0; i<root; i++) 
            {
                if (i%2 == 0)
                    printf("%c", '|');
                else
                    printf(" ");

            }

            if (dp->d_type != DT_DIR)
            {
                printf("|-- " ANSI_COLOR_BLUE "%s\n" ANSI_COLOR_RESET, dp->d_name);
            }
            else
            {
                printf("|-- " ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, dp->d_name);
            }

            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            tree(path, root + 2);
        }
    }

    closedir(dir);
}