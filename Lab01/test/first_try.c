#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    int out_desc = open("Output_Text.txt", O_WRONLY);
    int in_desc = open("Input_Text.txt", O_RDONLY);
    
    char content[100];

    int len = read(in_desc, content, 100);
    content[len] = '\0';

    int stdout_rw = write(1, content, len); // standard output write
    int desc_wr = write(out_desc, content, len); // file output write

    if(stdout_rw < 0 || desc_wr < 0) {
        write(2, "Error", 5);
    }

    return 0;
}