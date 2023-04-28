#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "auxiliares.h"

int main(){
    int fd;
    Proc * inicio = inicializar_lista();
    mkfifo("recebe_clientes", 0666);
    while (1)
    {        
        
        fd = open("recebe_clientes", O_RDONLY);
        

        char clienteAux[7];
        read(fd,clienteAux, sizeof(clienteAux));
        int pidInt = atoi(clienteAux);

        adicionar_processo(&inicio,pidInt,NULL,0);

        if(fork() == 0){
            int meuPid = pidInt;
            char Ncliente[7];
            sprintf(Ncliente,"%d", meuPid);

            int fdFilho;
            char ** args = NULL;
            int arg_count = 0;

            fdFilho = open(Ncliente, O_RDONLY);

            char tamanho[3];
            read(fdFilho,tamanho, 3);

            char buffer[200];
            read(fdFilho,buffer,atoi(tamanho));

            unsigned long long timestamp;
            read(fdFilho,&timestamp,sizeof(unsigned long long));

            Proc* current;

            if (fork()==0){
                current = inicio;

                while (current != NULL && current->pid!= meuPid) {
                    current = current->next;
                }

                if (current != NULL)
                {
                    strcpy(current->prog,buffer);
                    current->timestamp = timestamp;
                }
                else {printf("Pid inexistente: %d", meuPid);
                      exit(1);}
                exit(0);
                 
            }

                args = (char**) malloc(sizeof(char*));

                args[arg_count++] = strtok(buffer, " ");

            while(args[arg_count-1]!=NULL){
                args = (char**) realloc(args, (arg_count + 1) * sizeof(char*));
                args[arg_count++] = strtok(NULL, " ");
            } 


            close(fdFilho);
            int resposta = open(Ncliente, O_WRONLY);

            
            if (fork()==0){
                dup2(resposta,STDOUT_FILENO);
                execvp(args[0], args);
                free(args);
                exit(0);
            }
            close(resposta);
            int resposta1 = open(Ncliente,O_WRONLY);
            unsigned long long tempo_exec;
            read(resposta1,&tempo_exec,sizeof(unsigned long long));
            close(resposta1);

            if(current!=NULL){                
                current->finished = 1;
            }


            if(fork()==0){
                char nome[25];
                sprintf(nome,"data/%s.txt",Ncliente);
                int file = open(nome,O_WRONLY | O_CREAT | O_APPEND, 0666);
                char processo[150];
                sprint(processo,"%s;%s;%llu\n",Ncliente,buffer,tempo_exec);
                write(file,processo,strlen(processo));
                exit(0);
            }

            exit(0);
        }
       remove_finished_procs(&inicio);
    }
    close(fd);    
    return 0;
}