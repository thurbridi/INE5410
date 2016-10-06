#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10000

int global_counter = 0;
sem_t semaphore_lock;

void *sayHello(void *arg) {
  printf("Nova thread criada. TID = %ld!\n", pthread_self());
}

void *increment(void *arg) {
  for (int i = 0; i < 100; ++i) {
    // Lock
    sem_wait(&semaphore_lock);
    global_counter++;
    // Unlock
    sem_post(&semaphore_lock);
  }

}

int main(int argc, char **argv) {
  sem_init(&semaphore_lock, 0, 1);
  pthread_t threads [NUM_THREADS];
  for (long i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&threads[i], NULL, increment, (void *)i);
  }
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_join(threads[i], NULL);
  }
  sem_destroy(&semaphore_lock);
  printf("global_counter = %d\n", global_counter);

  return 0;
}