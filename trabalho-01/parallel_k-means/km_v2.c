#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define RANDNUM_W 521288629;
#define RANDNUM_Z 362436069;

unsigned int randum_w = RANDNUM_W;
unsigned int randum_z = RANDNUM_Z;

void srandnum(int seed) {
  unsigned int w, z;
  w = (seed * 104623) & 0xffffffff;
  randum_w = (w) ? w : RANDNUM_W;
  z = (seed * 48947) & 0xffffffff;
  randum_z = (z) ? z : RANDNUM_Z;
}

unsigned int randnum(void) {
  unsigned int u;
  randum_z = 36969 * (randum_z & 65535) + (randum_z >> 16);
  randum_w = 18000 * (randum_w & 65535) + (randum_w >> 16);
  u = (randum_z << 16) + randum_w;
  return (u);
}

typedef float* vector_t;

int npoints;
int dimension;
int ncentroids;
int maxthread;
int num_centroids_threads;
int num_populate_threads;
float mindistance;
int seed;
vector_t *data, *centroids;
int *map;
int *dirty;
int too_far;
int has_changed;
float npt, nct;
time_t timer;

struct args {
  int arg1;
  int arg2;
};

float v_distance(vector_t a, vector_t b) {
  int i;
  float distance = 0;
  for (i = 0; i < dimension; i++)
    distance +=  pow(a[i] - b[i], 2);
  return sqrt(distance);
}

static void *populatep(void *arg) {
  struct args *_arg = arg;
  int init = _arg->arg1;
  int end = _arg->arg2;

  for (int i = init; i < end; i++) {
    float distance = v_distance(centroids[map[i]], data[i]);
    /* Look for closest cluster. */
    for (int j = 0; j < ncentroids; j++) {
      /* Point is in this cluster. */
      if (j == map[i]) continue;
      float tmp = v_distance(centroids[j], data[i]);
      if (tmp < distance) {
        map[i] = j;
        distance = tmp;
        dirty[j] = 1;
      }
    }
    /* Cluster is too far away. */
    if (distance > mindistance)
      too_far = 1;
  }
}

static void populate(void) {

  time(&timer);

  too_far = 0;

  pthread_t *tid = malloc(sizeof(pthread_t)*num_populate_threads);
  struct args *arg = malloc(sizeof(struct args)*num_populate_threads);
  for (int i = 0; i < num_populate_threads; ++i) {
    arg[i].arg1 = i*npt;
    arg[i].arg2 = (i+1)*npt;
    pthread_create(&tid[i], NULL, populatep, (void *)&arg[i]);
  }

  for (int i = 0; i < num_populate_threads; ++i) {
    pthread_join(tid[i] ,NULL);
  }

  free(tid);
  free(arg);

  printf("Populate\n    Timer: %d\n", time(NULL) - timer);

}

static void *compute_centroidsp(void *arg) {
  struct args *_arg = arg;
  int init = _arg->arg1;
  int end = _arg->arg2;
  for (int i = init; i < end; i++) {
    if (!dirty[i]) continue;
    memset(centroids[i], 0, sizeof(float) * dimension);
    /* Compute cluster's mean. */
    int population = 0;
    for (int j = 0; j < npoints; j++) {
      if (map[j] != i) continue;
      for (int k = 0; k < dimension; k++)
        centroids[i][k] += data[j][k];
      population++;
    }
    if (population > 1) {
      for (int k = 0; k < dimension; k++)
        centroids[i][k] *= 1.0/population;
    }
    has_changed = 1;
  }
}

static void compute_centroids(void) {
  time(&timer);

  has_changed = 0;
  /* Compute means. */

  pthread_t *tid = malloc(sizeof(pthread_t)*num_centroids_threads);
  struct args *arg = malloc(sizeof(struct args)*num_centroids_threads);
  for (int i = 0; i < num_centroids_threads; ++i) {
    arg[i].arg1 = i*nct;
    arg[i].arg2 = (i+1)*nct;
    pthread_create(&tid[i], NULL, compute_centroidsp, (void *)&arg[i]);
  }

  for (int i = 0; i < num_centroids_threads; ++i) {
    pthread_join(tid[i] ,NULL);
  }

  free(tid);
  free(arg);

  memset(dirty, 0, ncentroids * sizeof(int));

  printf("Compute\n    Timer: %d\n", time(NULL) - timer);
}

int* kmeans(void) {
  int j;
  //too_far = 0;
  has_changed = 0;
  /*-----------------------------------*/
  if (!(map  = calloc(npoints, sizeof(int))))
    exit (1);
  if (!(dirty = malloc(ncentroids*sizeof(int))))
    exit (1);
  if (!(centroids = malloc(ncentroids*sizeof(vector_t))))
    exit (1);
  /*-----------------------------------*/
  for (int i = 0; i < ncentroids; i++)
    centroids[i] = malloc(sizeof(float) * dimension);
  for (int i = 0; i < npoints; i++)
    map[i] = -1;
  /*-----------------------------------*/
  for (int i = 0; i < ncentroids; i++) {
    dirty[i] = 1;
    j = randnum() % npoints;
    for (int k = 0; k < dimension; k++)
      centroids[i][k] = data[j][k];
    map[j] = i;
  }
  /*-----------------------------------*/
  /* Map unmapped data points. */
  for (int i = 0; i < npoints; i++)
    if (map[i] < 0)
      map[i] = randnum() % ncentroids;
  /*-----------------------------------*/
  do { /* Cluster data. */
    populate();
    compute_centroids();
  } while (too_far && has_changed);
  /*-----------------------------------*/
  for (int i = 0; i < ncentroids; i++)
    free(centroids[i]);
  free(centroids);
  free(dirty);

  return map;
}

int main(int argc, char **argv) {
  int i, j, tmp;

  if (argc != 7) {
    printf("Usage: npoints dimension ncentroids mindistance seed maxthread\n");
    exit (1);
  }

  npoints = atoi(argv[1]);
  dimension = atoi(argv[2]);
  ncentroids = atoi(argv[3]);
  mindistance = atoi(argv[4]);
  seed = atoi(argv[5]);
  maxthread = atoi(argv[6]);
  if (npoints < maxthread) {
    num_populate_threads = npoints;
  } else {
    num_populate_threads = maxthread;
  }

  if (ncentroids < maxthread) {
    num_centroids_threads = ncentroids;
  } else {
    num_centroids_threads = maxthread;
  }

  npt = (float) npoints / (float) maxthread;
  nct = (float) ncentroids / (float) maxthread;

  srandnum(seed);

  if (!(data = malloc(npoints*sizeof(vector_t))))
    exit(1);

  for (i = 0; i < npoints; i++) {
    data[i] = malloc(sizeof(float) * dimension);
    for (j = 0; j < dimension; j++)
      data[i][j] = randnum() & 0xffff;
  }

  map = kmeans();

  for (i = 0; i < ncentroids; i++) {
    printf("\nPartition %d:\n", i);
    for (j = 0; j < npoints; j++)
      if(map[j] == i)
        printf("%d ", j);
  }
  printf("\n");

  free(map);
  for (i = 0; i < npoints; i++)
    free(data[i]);
  free(data);

  return (0);
}
