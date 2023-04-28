#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include "auxiliares.h"

Proc* inicializar_lista() {
    return NULL;
}

void adicionar_processo(Proc** inicio, int pid, char* prog, unsigned long long timestamp) {
    Proc* novo_processo = malloc(sizeof(Proc));
    novo_processo->pid = pid;
    strcpy(novo_processo->prog, prog);
    novo_processo->timestamp = timestamp;
    novo_processo->finished = 0;
    novo_processo->next = NULL;
    novo_processo->tempo_exec = 0;
    
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
