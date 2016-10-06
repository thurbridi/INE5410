#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

#define BUFFER_SIZE 5

sem_t cheio, vazio;
sem_t lock_prod, lock_cons;

int i = 0;
int f = 0;

int buffer[BUFFER_SIZE];

int produz() {
  return rand() % 10;
}

int consome(int pos) {
  return buffer[pos];
}

void *produtor(void *arg) {
  int counter = 0;
  while (counter < 15) {
    sem_wait(&vazio);
    sem_wait(&lock_prod);
    f = (f + 1) % BUFFER_SIZE;
    buffer[f] = produz();
    printf("%d Produzido em %d\n", buffer[f], f);
    sem_post(&lock_prod);
    sem_post(&cheio);
    counter ++;
  }
}

void *consumidor(void *arg) {
  int counter = 0;
  while (counter < 15) {
    sem_wait(&cheio);
    sem_wait(&lock_cons);
    i = (i + 1) % BUFFER_SIZE;
    int consumido = consome(i);
    printf("%d Consumido em %d\n", consumido, i);
    sem_post(&lock_cons);
    sem_post(&vazio);
    counter++;
  }
}

int main(int argc, char **argv) {
  sem_init(&cheio, 0, 0);
  sem_init(&vazio, 0, BUFFER_SIZE);
  sem_init(&lock_prod, 0, 1);
  sem_init(&lock_cons, 0, 1);

  pthread_t thread_produtor;
  pthread_t thread_consumidor;

  pthread_create(&thread_produtor, NULL, produtor, NULL);
  pthread_create(&thread_consumidor, NULL, consumidor, NULL);

  pthread_join(thread_produtor, NULL);
  pthread_join(thread_consumidor, NULL);

  sem_destroy(&lock_prod);
  sem_destroy(&lock_cons);
  sem_destroy(&cheio);
  sem_destroy(&vazio);
  return 0;
}