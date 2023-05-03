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

    int fd = open("recebe_clientes", O_WRONLY);
    char aux[7];
    sprintf(aux,"%06d", getpid());
    write(fd,aux,strlen(aux)+1);

    char nCliente[7];
    sprintf(nCliente,"%d", getpid());
    printf("Cliente: %s\n", nCliente);
    mkfifo(nCliente, 0666);

    if(strcmp(argv[1],"execute") == 0 && strcmp(argv[2],"-u") == 0){
        int fdCliente = open(nCliente, O_WRONLY);
        if ((argc - 3) <= 0) {
            printf("Erro: nenhum argumento fornecido\n");
            return 1;
        }
        int len = strlen(argv[3]);
        printf("Programa: %s\n\n---------------------------------------\n\n", argv[3]);

        unsigned long long timestamp_inicio= calcula_timestamp();

        
        char pedido[len + 4];
        sprintf(pedido,"u%03d%s",len+1,argv[3]);
        write(fdCliente,pedido,strlen(pedido)+1);
        close(fdCliente);
        int resposta = open(nCliente,O_RDONLY);

        char bufferResposta[512];
        int bytes;

        while((bytes = read(resposta,bufferResposta,sizeof(bufferResposta))) > 0){
            write(STDOUT_FILENO,bufferResposta,bytes);

        }
        close(resposta);

        unsigned long long timestamp_fim= calcula_timestamp();

        unsigned long long tempo_exec = timestamp_fim - timestamp_inicio;
        int resposta1 = open(nCliente,O_WRONLY);
        write(resposta1,&tempo_exec,sizeof(tempo_exec));
        close(resposta1);

        printf("\n\nTempo de execução do programa: %llums\n", tempo_exec);

        
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
    else puts("Indique uma opção valida");
        
close(fd);
return 0;
}