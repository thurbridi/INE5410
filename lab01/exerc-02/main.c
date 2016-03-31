#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  pid_t id_filho_1 = fork();
  if (id_filho_1 > 0) {
    printf("Processo pai %d criou %d\n", getpid(), id_filho_1);
    pid_t id_filho_2 = fork();
    if (id_filho_2 > 0) {
      printf("Processo pai %d criou %d\n", getpid(), id_filho_2);
      pid_t id_filho_3 = fork();
      if (id_filho_3 > 0) {
        printf("Processo pai %d criou %d\n", getpid(), id_filho_3);
        pid_t id_filho_4 = fork();
        if (id_filho_4 > 0) {
          printf("Processo pai %d criou %d\n", getpid(), id_filho_4);
        }
      }
    }
  }

  wait(NULL);
  return 0;
}