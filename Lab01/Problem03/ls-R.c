#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void function(DIR *dirPointer)
{
    DIR *dirPointer = opendir(".");
    if (dirPointer != NULL)
    {
        struct dirent *dp;
        while ((dp = readdir(dirPointer)) != NULL)
        {
            write(2, dp->d_name, strlen(dp->d_name));
            write(2, " ", 1);
        }
        write(2, "\n", 1);
        function(dirPointer);
    }
}

int main()
{
    DIR *dirPointer = opendir(".");
    function(dirPointer);
}