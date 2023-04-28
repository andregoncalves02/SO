typedef struct {
  int pid;
  char prog[100];
  unsigned long long timestamp;
  int finished;
  unsigned long long tempo_exec;
  struct Proc *next;
} Proc;

Proc* inicializar_lista();

void adicionar_processo(Proc** inicio, int pid, char* prog, unsigned long long timestamp);

void remover_elemento(Proc **inicio, int pid);

void remove_finished_procs(Proc **inicio);

unsigned long long calcula_timestamp();