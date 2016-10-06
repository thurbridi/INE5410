#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, comm_size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  int msg = 0;
  if (rank == 0) {
    msg = 1;
  }

  MPI_Bcast(&msg, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank != 0 && msg == 1) {
    MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    msg = 0;
  }

  if (rank == 0) {
    int i = 0;
    while (i < comm_size - 1) {
      MPI_Recv(&rank, 1, MPI_INT, MPI_ANY_SOURCE, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("Received message from rank: %d\n", rank);
      i++;
    }
  }
  MPI_Finalize();
  return 0;
}