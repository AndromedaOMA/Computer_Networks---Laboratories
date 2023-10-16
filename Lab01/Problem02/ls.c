#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int main()
{
    DIR *dirPointer = opendir(".");
    struct dirent *dp;
    while ((dp = readdir(dirPointer)) != NULL)
    {
        write(2, dp->d_name, strlen(dp->d_name));
        write(2, "\n", 1);
    }
}