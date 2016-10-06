#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3

void *sayHello(void *arg) {
  printf("Nova thread criada. TID = %ld!\n", pthread_self());
}

int main(int argc, char **argv) {
  pthread_t threads [NUM_THREADS];
  for (long i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, sayHello, (void *)i);
  }
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }
  pthread_exit(NULL);
  return 0;
}