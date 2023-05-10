#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "auxiliares.h"

int main(){
    int fd;
    Proc * inicio = NULL;
    mkfifo("recebe_clientes", 0666);
    while (1)
    {        
        
        fd = open("recebe_clientes", O_RDONLY);
        

        char clienteAux[7];
        read(fd,clienteAux, sizeof(clienteAux));
        int pidInt = atoi(clienteAux);

        int meuPid = pidInt;
        char Ncliente[7];
        sprintf(Ncliente,"%d", meuPid);

        int fdFilho;

        fdFilho = open(Ncliente, O_RDONLY);
        char flag;
        read(fdFilho,&flag,1);

        if(flag=='e'){
            adiciona_processo(&inicio,pidInt);
            char tamanho[3];
            read(fdFilho,tamanho, 3);

            char buffer[200];
            read(fdFilho,buffer,atoi(tamanho));

            char bufferAux[strlen(buffer)+1];
            strcpy(bufferAux,buffer);

            unsigned long long timestamp;
            read(fdFilho,&timestamp,sizeof(unsigned long long));

            alterar_nodo(&inicio, meuPid, bufferAux,timestamp);
                                                    
            close(fdFilho);

            int resposta1 = open(Ncliente,O_RDONLY);
            unsigned long long tempo_exec;
            read(resposta1,&tempo_exec,sizeof(unsigned long long));
            close(resposta1);
            alterar_finished(&inicio, meuPid);

            if(fork()==0){
                char nome[25];
                    sprintf(nome,"data/%s.txt",Ncliente);
                    if(access("data", F_OK) != 0) {
                        if(mkdir("data", S_IRWXU) != 0) {
                                perror("Erro ao criar diretório");
                                exit(1);
                            }
                        }
                    int file = open(nome,O_WRONLY | O_CREAT | O_APPEND, 0666);
                    if (file == -1) { 
                            perror("Erro ao abrir arquivo");
                            exit(1);
                        }
                    char processo[512];
                    sprintf(processo,"%s;%s;%llu\n",Ncliente,bufferAux,tempo_exec);
                    write(file,processo,strlen(processo));
                    close(file); 
                    exit(0);
                    }
            }
            else if(flag == 's'){               
                close(fdFilho);
                if(fork() == 0){
                int resposta = open(Ncliente, O_WRONLY);
                imprimir_lista(inicio,resposta);
                close(resposta);
                }
            }
        close(fd);
        }
    free_lista(inicio);    
    return 0;
}