typedef struct Proc {
  int pid;
  char prog[150];
  unsigned long long timestamp;
  int finished;
  unsigned long long tempo_exec;
  struct Proc* next;
} Proc;

Proc* inicializar_lista();

void adiciona_processo(Proc **inicio, int pid, char* prog, unsigned long long timestamp);

void remover_elemento(Proc **inicio, int pid);

void remove_finished_procs(Proc **inicio);

unsigned long long calcula_timestamp();

void imprimir_lista(Proc *head, int descritor);

void imprimir_lista_time(Proc *head, int descritor, char* pids);

void imprimir_lista_uniq(Proc *head, int descritor, char* pids);

void imprimir_lista_commands(Proc *head, int descritor, char* pids, char* prog);

void free_lista(Proc *head);

void execute_u(char* prog);

void alterar_finished(Proc **inicio, int pid, unsigned long long temp_exec);

void execute_pipeline(char* pipeline);

int existe_pid(Proc **inicio, int pid);

char* devolve_prog(Proc **inicio, int pid);