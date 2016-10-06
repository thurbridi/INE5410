#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10000

int contador_global = 0;

void *sayHello(void *arg) {
  printf("Nova thread criada. TID = %ld!\n", pthread_self());
}

void *incrementa(void *arg) {
  for (int i = 0; i < 100; ++i) {
    contador_global++;
  }

}

int main(int argc, char **argv) {
  pthread_t threads [NUM_THREADS];
  for (long i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, incrementa, (void *)i);
  }
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }

  printf("contador_global = %d\n", contador_global);

  return 0;
}