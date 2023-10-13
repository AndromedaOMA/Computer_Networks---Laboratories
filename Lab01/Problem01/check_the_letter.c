#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    char character[2];
    int len = read(0, character, 1);
    character [len]='\0';

    if (len == -1)
    {
        perror("Error1!");
        return 1;
    }

    if (character[0] >= 'A' && character[0] <= 'Z' || character[0] >= 'a' && character[0] <= 'z')
    {
        int WRITE = write(1, character, 1);
        if (WRITE == -1)
        {
            perror("Error2!");
            return 2;
        }
    }
    else
    {
        int WRITE = write(1, "ERR!", 5);
        if (WRITE == -1)
        {
            perror("Error3!");
            return 2;
        }
    }
    return 0;
}