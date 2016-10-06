#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

void *sayHello(void *arg) {
  printf("Nova thread criada. TID = %d\n", pthread_self());
}

int main(int argc, char **argv) {
  pthread_t thread;
  long int arg = 123;
  pthread_create(&thread, NULL, sayHello, (void *)arg);
  pthread_join(thread, NULL);
  pthread_exit(NULL);
  return 0;
}