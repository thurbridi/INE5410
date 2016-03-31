#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char const *argv[]) {
  fork();

  printf("Novo processo criado!\n");

  return 0;
}