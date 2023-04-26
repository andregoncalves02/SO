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

    if(fork() == 0){
        char clienteAux[7];
        read(fd,clienteAux, sizeof(clienteAux));

        int aux = atoi(clienteAux);
        char Ncliente[7];
        sprintf(Ncliente,"%d", aux);

        int fdFilho;
        char ** args = NULL;
        int arg_count = 0;

        fdFilho = open(Ncliente, O_RDONLY);
        if(fdFilho<0){puts("erro");}

        char tamanho[3];
        read(fdFilho,tamanho, 3);

        char buffer[200];
        read(fdFilho,buffer,atoi(tamanho));

            args = (char**) malloc(sizeof(char*));

            args[arg_count++] = strtok(buffer, " ");

        while(args[arg_count-1]!=NULL){
            args = (char**) realloc(args, (arg_count + 1) * sizeof(char*));
            args[arg_count++] = strtok(NULL, " ");
        } 


        close(fdFilho);
        int resposta = open(Ncliente, O_WRONLY);

        dup2(resposta,STDOUT_FILENO);
        if (fork()==0){
            execvp(args[0], args);
            free(args);
        }
        close(resposta);
        exit(0);
    }
    close(fd);
     return 0;
}
