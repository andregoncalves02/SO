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

void adiciona_processo(Proc **inicio, int pid) {
    Proc *novo_processo = malloc(sizeof(Proc));
    novo_processo->pid = pid;
    novo_processo->prog = NULL;
    novo_processo->timestamp = 0;
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
            printf("%s",atual->prog);
            sprintf(buffer, "%d;%s;%llu\n", atual->pid, atual->prog, tempo_exec);
            write(descritor, buffer, strlen(buffer));
        }
        atual = atual->next;
    }
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

void execute_u(char* fifo , char** args){
    int resposta = open(fifo, O_WRONLY);                                
    if (resposta < 0) {
        perror("Erro ao abrir o arquivo do cliente");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("Erro ao criar processo filho");
        exit(1);
    } else if (pid == 0) {
        // Processo filho
        dup2(resposta, STDOUT_FILENO);
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
    close(resposta);
}

void alterar_nodo(Proc **inicio, int pid, char *prog, unsigned long long timestamp) {
    Proc* current = *inicio;
    while (current != NULL && current->pid != pid) {
        current = current->next;
    }
    if (current != NULL) {
        current->timestamp = timestamp;
        current->finished = 0;
        current->prog = malloc(strlen(prog) + 1); 
        strcpy(current->prog, prog);
    } else {
        printf("Pid inexistente: %d", pid);
    }
}

void alterar_finished(Proc **inicio, int pid) {
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
        }
    } else {
        printf("Pid inexistente: %d", pid);
    }
}


