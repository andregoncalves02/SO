#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    int fd;
    mkfifo("recebe_clientes", 0666);
    fd = open("recebe_clientes", O_RDONLY);
    char Ncliente[50];
    int bytes = read(fd,Ncliente, sizeof(Ncliente)); 
    pid_t pid = fork();

    if(pid == 0){
        int fdFilho;
        char buffer[1024];
        char ** args = NULL;
        int arg_count = 0;
        mkfifo(Ncliente, 0666);
        fdFilho = open(Ncliente, O_RDONLY);
        int bytes1 = read(fdFilho,buffer, sizeof(buffer));

        args = (char**) malloc(sizeof(char*));
        args[arg_count++] = strtok(buffer, " ");
        while(args[arg_count-1]!=NULL){
            args = (char**) realloc(args, (arg_count + 1) * sizeof(char*));
            args[arg_count++] = strtok(NULL, " "); 
        }
        execvp(args[0], args);
        free(args);
        exit(0);
    }
     return 0;
}
