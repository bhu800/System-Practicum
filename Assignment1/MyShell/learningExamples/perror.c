#include <stdio.h>

int main()
{
    FILE *fp;

    rename("file.txt", "newfile.txt");

    fp = fopen("file.txt", "r");
    if (fp == NULL)
    {
        perror("ERROR in entered file name ");
        return(-1);
    }
    fclose(fp);

    return(0);
}