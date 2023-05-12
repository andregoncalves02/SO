#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "auxiliares.h"

int main() {
    int fd;
    Proc *inicio = NULL;
    mkfifo("recebe_clientes", 0666);
    
    while (1) {
        fd = open("recebe_clientes", O_RDONLY);

        char clienteAux[7];
        read(fd, clienteAux, sizeof(clienteAux));

        int pidInt = atoi(clienteAux);
        char Ncliente[7];
        sprintf(Ncliente, "%d", pidInt);

        if (existe_pid(&inicio, pidInt) == 0){
            int fdFilho;
        
            fdFilho = open(Ncliente, O_RDONLY);
            
            char flag;
            read(fdFilho, &flag, 1);

            if (flag == 'e') {
                char tamanho[3];
                read(fdFilho, tamanho, 3);

                int len = atoi(tamanho);
                char buffer[len+1];
                read(fdFilho, buffer, len+1);

                char bufferAux[len+1];
                strcpy(bufferAux, buffer);

                unsigned long long timestamp;
                read(fdFilho, &timestamp, sizeof(unsigned long long));

                close(fdFilho);
            
                adiciona_processo(&inicio, pidInt, bufferAux, timestamp);
            }
            else if (flag == 's') {
                close(fdFilho);
            
                if (fork() == 0) {
                    int resposta = open(Ncliente, O_WRONLY);
                    imprimir_lista(inicio, resposta);
                    close(resposta);
                }
            }
            else if (flag == 't' || flag == 'u') {
                printf("%c",flag);
                char tamanho[3];
                read(fdFilho,tamanho,3);
                
                int len = atoi(tamanho);
                char pids[len+1];
                read(fdFilho,pids,len+1);
                close(fdFilho);
                
            
                if (fork() == 0) {
                    int resposta = open(Ncliente, O_WRONLY);
                    if(flag == 't'){
                        imprimir_lista_time(inicio, resposta, pids);
                    }
                    else if(flag == 'u'){
                        imprimir_lista_uniq(inicio, resposta, pids);  
                    }
                    close(resposta);
                    exit(0);
                }
            }
        }
            else {
                int resposta1 = open(Ncliente, O_RDONLY);
                unsigned long long tempo_exec;
                read(resposta1, &tempo_exec, sizeof(unsigned long long));
                close(resposta1);
                alterar_finished(&inicio, pidInt, tempo_exec);

                if (fork() == 0) {
                    char nome[25];
                    sprintf(nome, "data/%s.txt", Ncliente);
                    
                    if (access("data", F_OK) != 0) {
                        if (mkdir("data", S_IRWXU) != 0) {
                            perror("Erro ao criar diretório");
                            exit(1);
                        }
                    }
                    
                    int file = open(nome, O_WRONLY | O_CREAT | O_APPEND, 0666);
                    if (file == -1) {
                        perror("Erro ao abrir arquivo");
                        exit(1);
                    }
                    
                    char processo[512];
                    char* prog = devolve_prog(&inicio,pidInt);
                    sprintf(processo, "%s;%s;%llu\n", Ncliente, prog, tempo_exec);
                    write(file, processo, strlen(processo));
                    close(file);

                    exit(0);
                }
            } 
        
        close(fd);
    }
    
    free_lista(inicio);
    return 0;
}
