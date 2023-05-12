#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include "auxiliares.h"

int main(int argc, char * argv[]){

    char nCliente[7];
    sprintf(nCliente,"%d", getpid());
    printf("Running PID: %s\n\n", nCliente);
    mkfifo(nCliente, 0666);

    int fd = open("recebe_clientes", O_WRONLY);
    char aux[7];
    sprintf(aux,"%06d", getpid());
    write(fd,aux,strlen(aux)+1);
    close(fd);


    if(strcmp(argv[1],"execute") == 0){
        char type[2];
        strcpy(type, argv[2]);
        
        int fdCliente = open(nCliente, O_WRONLY);
        if ((argc - 3) <= 0) {
            printf("Erro: nenhum argumento fornecido\n");
            return 1;
        }
        int len = strlen(argv[3]);

        char pedido[len + 6];
        sprintf(pedido,"e%03d%s",len+1,argv[3]);
        write(fdCliente,pedido,strlen(pedido)+1);
        unsigned long long timestamp_inicio= calcula_timestamp();
        write(fdCliente,&timestamp_inicio, sizeof(unsigned long long));
        close(fdCliente);
        
        if(strcmp("-u",type)==0){
            execute_u(argv[3]);
        }
        else{
            execute_pipeline(argv[3]);
        }


        unsigned long long timestamp_fim= calcula_timestamp();
        int fd2 = open("recebe_clientes", O_WRONLY);
        write(fd2,aux,strlen(aux)+1);
        close(fd2);

        unsigned long long tempo_exec = timestamp_fim - timestamp_inicio;
        int resposta1 = open(nCliente,O_WRONLY);
        write(resposta1,&tempo_exec,sizeof(tempo_exec));
        close(resposta1);
        

        printf("\n\nEnded in %llu ms\n", tempo_exec);

        
    }
    else if (strcmp(argv[1],"status") == 0)
        {
        int fdCliente = open(nCliente, O_WRONLY);
        write(fdCliente,"s",2);
        close(fdCliente);

        int resposta = open(nCliente, O_RDONLY);

        char bufferResposta[512];
        int bytes;
        while((bytes = read(resposta,bufferResposta,sizeof(bufferResposta))) > 0){
            write(STDOUT_FILENO,bufferResposta,bytes);
        }

        close(resposta);

    }
    else if(strcmp(argv[1],"stats-time")==0){
        int fdCliente = open(nCliente, O_WRONLY);
        char pid_string[400];

        for (int i = 2; i < argc; i++) {
            strcat(pid_string, argv[i]);
            strcat(pid_string, " ");
        }

        int tamanho = strlen(pid_string);
        char pedido[tamanho+5];
        sprintf(pedido,"t%03d%s",tamanho,pid_string);
        write(fdCliente,pedido,strlen(pedido)+1);
        close(fdCliente);

        int resposta = open(nCliente, O_RDONLY);

        char bufferResposta[512];
        int bytes;
        while((bytes = read(resposta,bufferResposta,sizeof(bufferResposta))) > 0){
            write(STDOUT_FILENO,bufferResposta,bytes);
        }

        close(resposta);



    }
    else if(strcmp(argv[1],"stats-uniq")==0){
        int fdCliente = open(nCliente, O_WRONLY);
        char pid_string[400];

        for (int i = 2; i < argc; i++) {
            strcat(pid_string, argv[i]);
            strcat(pid_string, " ");
        }


        int tamanho = strlen(pid_string);
        char pedido[tamanho+5];
        sprintf(pedido,"u%03d%s",tamanho,pid_string);
        write(fdCliente,pedido,strlen(pedido)+1);
        close(fdCliente);

        int resposta = open(nCliente, O_RDONLY);

        char bufferResposta[512];
        int bytes;
        while((bytes = read(resposta,bufferResposta,sizeof(bufferResposta))) > 0){
            write(STDOUT_FILENO,bufferResposta,bytes);
        }

        close(resposta);



    }
    else puts("Indique uma opção valida");
        
return 0;
}