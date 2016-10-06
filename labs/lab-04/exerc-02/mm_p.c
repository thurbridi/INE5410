#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

#define NRA 1000                 /* number of rows in matrix A */
#define NCA 1000                 /* number of columns in matrix A */
#define NCB 1000                 /* number of columns in matrix B */

int main (int argc, char *argv[])
{
  int i, j, k;
  omp_set_num_threads(4);

  /* matrix A to be multiplied */
  double **a = (double **) malloc(sizeof(double *) * NRA);
  for (i = 0; i < NRA; i ++)
    a[i] = (double *) malloc(sizeof(double) * NCA);

  /* matrix B to be multiplied */
  double **b = (double **) malloc(sizeof(double *) * NCA);
  for (i = 0; i < NCA; i ++)
    b[i] = (double *) malloc(sizeof(double) * NCB);

  /* result matrix C */
  double **c = (double **) malloc(sizeof(double *) * NRA);
  for (i = 0; i < NRA; i ++)
    c[i] = (double *) malloc(sizeof(double) * NCB);

  printf("Initializing matrices...\n");

  /*** Initialize matrices ***/
  #pragma omp parallel for schedule(static) private(j)
  for (i=0; i<NRA; i++)
    for (j=0; j<NCA; j++)
      a[i][j]= i+j;

  #pragma omp parallel for schedule(static) private(j)
  for (i=0; i<NCA; i++)
    for (j=0; j<NCB; j++)
      b[i][j]= i*j;

  #pragma omp parallel for schedule(static) private(j)
  for (i=0; i<NRA; i++)
    for (j=0; j<NCB; j++)
      c[i][j]= 0;

  /*** Do the matrix-matrix multiplication ***/
  #pragma omp parallel for schedule(static) private(j, k)
  for (i=0; i<NRA; i++)
    for(j=0; j<NCB; j++)
      for (k=0; k<NCA; k++)
        c[i][j] += a[i][k] * b[k][j];

  /*** Print results ***/
  printf("******************************************************\n");
  printf("Result Matrix:\n");
  for (i=0; i<NRA; i++)
  {
    for (j=0; j<NCB; j++)
      printf("%10.2f  ", c[i][j]);
    printf("\n");
  }
  printf("******************************************************\n");
  printf ("Done.\n");

  for (i = 0; i < NRA; i ++)
    free(a[i]);
  free(a);

  for (i = 0; i < NCA; i ++)
    free(b[i]);
  free(b);

  for (i = 0; i < NRA; i ++)
    free(c[i]);
  free(c);

  return 0;
}
