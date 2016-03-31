#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  pid_t pid_1 = fork();
  
  if (pid_1 == 0) {
    printf("Processo %d filho de processo %d\n", getpid(), getppid());

    pid_t pid_3 = fork();
    if (pid_3 == 0) {
      printf("Processo %d filho de processo %d\n", getpid(), getppid());
    } else if (pid_3 > 0) {
      pid_t pid_4 = fork();

      if (pid_4 == 0) {
        printf("Processo %d filho de processo %d\n", getpid(), getppid());
      }      
    }
  } else if (pid_1 > 0) {
    pid_t pid_2 = fork();
    
    if (pid_2 == 0) {
      printf("Processo %d filho de processo %d\n", getpid(), getppid());

      pid_t pid_5 = fork();
      if (pid_5 == 0) {
        printf("Processo %d filho de processo %d\n", getpid(), getppid());
      } else if (pid_5 > 0) {
        pid_t pid_6 = fork();

        if (pid_6 == 0) {
        printf("Processo %d filho de processo %d\n", getpid(), getppid());
        }      
      }
    }
  }

  wait(NULL);
  return 0;
}