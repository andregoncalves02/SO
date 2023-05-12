#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "auxiliares.h"

Proc* inicializar_lista() {
    return NULL;
}

void adiciona_processo(Proc **inicio, int pid, char* prog, unsigned long long timestamp) {
    Proc *novo_processo = malloc(sizeof(Proc));
    novo_processo->pid = pid;
    strcpy(novo_processo->prog,prog);
    novo_processo->timestamp = timestamp;
    novo_processo->finished = 0;
    novo_processo->tempo_exec = 0;
    novo_processo->next = NULL;

    if (*inicio == NULL) {
        *inicio = novo_processo;
    } else {
        Proc* ultimo = *inicio;
        while (ultimo->next != NULL) {
            ultimo = ultimo->next;
        }
        ultimo->next = novo_processo;
    }
}

void remover_elemento(Proc **inicio, int pid) {
    Proc *atual = *inicio;
    Proc *anterior = NULL;

    while (atual != NULL && atual->pid != pid) {
        anterior = atual;
        atual = atual->next;
    }

    if (atual == NULL) {
        return;
    }

    if (anterior == NULL) {
        *inicio = atual->next;
    } 

    else {
        anterior->next = atual->next;
    }
    free(atual);
}

unsigned long long calcula_timestamp(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000;
}


void remove_finished_procs(Proc **head) {
  Proc *current = *head;
  Proc *prev = NULL;

  while (current != NULL) {
    if (current->finished == 1) {
      if (prev == NULL) {
        *head = current->next;
      } else {
        prev->next = current->next;
      }
      Proc *temp = current;
      current = current->next;
      free(temp);
    } else {
      prev = current;
      current = current->next;
    }
  }
}


void imprimir_lista(Proc *head, int descritor) {
    unsigned long long now = calcula_timestamp();
    Proc *atual = head;
    char buffer[400];
    while (atual != NULL) {
        if (atual->finished == 0) {
            unsigned long long tempo_exec = now - atual->timestamp;
            sprintf(buffer, "%d;%s;%llu\n", atual->pid, atual->prog, tempo_exec);
            write(descritor, buffer, strlen(buffer));
        }
        atual = atual->next;
    }
}
void imprimir_lista_time(Proc *head, int descritor, char* pids) {
    unsigned long long tempo = 0;
    char buffer[400];
    char* token = strtok(pids, " ");
    
    while (token != NULL) {
        int pid = atoi(token);
        Proc *atual = head;
        while (atual != NULL) {
            if (atual->pid == pid) {
                tempo += atual->tempo_exec;
                break;
            }
            atual = atual->next;
        }
        
        token = strtok(NULL, " ");
    }
    
    sprintf(buffer, "Tempo total de execução dos PIDs: %llu ms\n", tempo);
    write(descritor, buffer, strlen(buffer));
}
void imprimir_lista_uniq(Proc *head, int descritor, char* pids) {
    char* token = strtok(pids, " ");
    char bufferResposta[512];
    bufferResposta[0] = '\0';
    while (token != NULL) {
        int pid = atoi(token);
        Proc *atual = head;
        while (atual != NULL) {
            if (atual->pid == pid) {
                char buffer[200];
                    sprintf(buffer, "%s\n", atual->prog);
                    strcat(bufferResposta,buffer);
            }
            atual = atual->next;
        }
        
        token = strtok(NULL, " ");
    }
    
    write(descritor, bufferResposta, strlen(bufferResposta));
    
}

void free_lista(Proc* inicio) {
    Proc* atual = inicio;
    while (atual != NULL) {
        Proc* proximo = atual->next;
        free(atual->prog); 
        free(atual);      
        atual = proximo;
    }
}

void execute_u(char* prog){
    char ** args = NULL;
    int arg_count = 0;
    args = (char**) malloc(sizeof(char*));

                args[arg_count++] = strtok(prog, " ");

                while(args[arg_count-1]!=NULL){
                    args = (char**) realloc(args, (arg_count + 1) * sizeof(char*));
                    args[arg_count++] = strtok(NULL, " ");
                }
    pid_t pid = fork();
    if (pid < 0) {
        perror("Erro ao criar processo filho");
        exit(1);
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("Erro ao executar o programa");
        exit(1);
    }

    // Processo pai
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("Erro ao esperar pelo processo filho");
        exit(1);
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("Erro ao executar o programa\n");
        exit(1);
    }

    free(args);
}



void alterar_finished(Proc **inicio, int pid, unsigned long long temp_exec) {
    Proc* current = *inicio;
    while (current != NULL && current->pid != pid) {
        current = current->next;
    }
    if (current != NULL) {
        if(current->finished==1){
            current->finished=0;
        }
        else{
            current->finished = 1;
            current->tempo_exec = temp_exec;
        }
    } else {
        printf("Pid inexistente: %d", pid);
    }
}

void execute_pipeline(char* pipeline) {
    char comandos[10][100];
    char* commands[10][100];
    int command_count = 0;

    char *token = strtok(pipeline, "|");
    while (token != NULL) {
        strcpy(comandos[command_count], token);
        token = strtok(NULL, "|");
        command_count++;
    }

    for (int i = 0; i < command_count; i++) {
        char temp[100];
        strcpy(temp, comandos[i]);

        char* token = strtok(temp, " ");
        int j = 0;

        while (token != NULL) {
            commands[i][j] = strdup(token);
            token = strtok(NULL, " ");
            j++;
        }
        commands[i][j] = NULL;
    }

    int pipo[command_count-1][2];
    pid_t child_pids[command_count];

    for (int i = 0; i < command_count; i++) {
        if (i < command_count - 1) {
            pipe(pipo[i]);
        }

        child_pids[i] = fork();
        if (child_pids[i] == 0) {
            if (i == 0) {
                dup2(pipo[0][1], 1);
                close(pipo[0][0]);
                close(pipo[0][1]);
            } else if (i < command_count - 1) {
                dup2(pipo[i - 1][0], 0);
                close(pipo[i - 1][0]);

                dup2(pipo[i][1], 1);
                close(pipo[i][0]);
                close(pipo[i][1]);
            } else {
                dup2(pipo[i - 1][0], 0);
                close(pipo[i - 1][1]);
                close(pipo[i - 1][0]);
            }

            execvp(commands[i][0], commands[i]);

            perror("exec falhou");
            exit(1);
        }
    }

    for (int i = 0; i < command_count - 1; i++) {
        close(pipo[i][0]);
        close(pipo[i][1]);
    }

    for (int i = 0; i < command_count; i++) {
        waitpid(child_pids[i], NULL, 0);
    }

    for (int i = 0; i < command_count; i++) {
        for (int j = 0; commands[i][j] != NULL; j++) {
            free(commands[i][j]);
        }
    }
}


int existe_pid(Proc **inicio, int pid) {
    Proc* current = *inicio;
    while (current != NULL) {
        if (current->pid == pid) {
            return 1; // O pid existe na lista
        }
        current = current->next;
    }
    return 0;
}
char* devolve_prog(Proc **inicio, int pid) {
    Proc* current = *inicio;
    char aux[100];
    while (current != NULL && current->pid != pid) {
        current = current->next;
    }
    if (current != NULL) {
        strcpy(aux, current->prog);
        char* result = malloc(strlen(aux) + 1);
        strcpy(result, aux);
        return result;
    } else {
        printf("Pid inexistente: %d\n", pid);
        return NULL;
    }
}
