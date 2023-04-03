#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char * argv[]){
    int bytes;
    puts("Cliente ativo...");
    int fd = open("fifo", O_WRONLY);
    
    write(fd,argv[1],strlen(argv[1]));

    close(fd);
    return 0;
}