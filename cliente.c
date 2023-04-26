#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

int main(int argc, char * argv[]){
    int bytes;
    puts("Cliente ativo...");
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
        /*
        for (int i = 3; i < argc; i++)
        {
            len += strlen(argv[i]) + 1;
        }
        char* aux = (char *) malloc(sizeof(char)*len);
        int j;
        for (j = 3; j<argc-1; j++){
            strcat(aux,argv[j]);
            strcat(aux," ");
        }        
        strcat(aux, argv[j]);
        */
        printf("Programa: %s\n", argv[3]);

        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long timestamp_inicio= (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;

        printf("Timestamp em milissegundos: %llu\n\n-----------------------------------\n", timestamp_inicio);
        
        char pedido[len + 4];
        sprintf(pedido,"%03d%s",len+1,argv[3]);
        write(fdCliente,pedido,strlen(pedido)+1);
        close(fdCliente);
        int resposta = open(nCliente,O_RDONLY);

        char bufferResposta[512];
        int bytes;

        while((bytes = read(resposta,bufferResposta,sizeof(bufferResposta))) > 0){
            write(STDOUT_FILENO,bufferResposta,bytes);

        }

        gettimeofday(&tv, NULL);
        unsigned long long timestamp_fim= (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;

        unsigned long long tempo_exec = timestamp_fim - timestamp_inicio;
        printf("\n\nTempo de execução do programa: %lldms\n", tempo_exec);

        close(resposta);

        
    }
    

    close(fd);
    return 0;
}
