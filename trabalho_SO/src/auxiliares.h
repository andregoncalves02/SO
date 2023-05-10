typedef struct Proc {
  int pid;
  char* prog;
  unsigned long long timestamp;
  int finished;
  unsigned long long tempo_exec;
  struct Proc* next;
} Proc;

Proc* inicializar_lista();

void adiciona_processo(Proc** inicio, int pid);

void remover_elemento(Proc **inicio, int pid);

void remove_finished_procs(Proc **inicio);

unsigned long long calcula_timestamp();

void imprimir_lista(Proc *head, int descritor);

void free_lista(Proc *head);

void execute_u(char* prog);

void alterar_nodo(Proc **inicio, int pid, char *prog, unsigned long long timestamp);

void alterar_finished(Proc **inicio, int pid);

void execute_pipeline(char* pipeline);