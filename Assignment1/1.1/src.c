#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

long sizes[5] = {128, 1024, 65536, 1048576, 4194304};

int main( int argc, char *argv[])
{
  int myrank, size;
  double start_time, elapsed_time, bandwidth;
  MPI_Status status;
  // MPI_Request request[];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  for (size_t i = 0; i < 5; i++) {
      long num_bytes = sizes[i];
      char *arr;

      arr = malloc (sizeof(char) * num_bytes);
      memset (arr, '0', num_bytes * sizeof(char));

      start_time = MPI_Wtime ();
      if (myrank == 0) {
          // printf("Rank %d sent %ld bytes\n", myrank, num_bytes);
          /* Send a buffer of num_bytes length to all the nodes (except root itself)
           * from the root node */
          for (size_t i = 0; i < 100; i++) {
              MPI_Send(arr, num_bytes, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
          }
      } else if (myrank == 1) {
          char recvarr[num_bytes];
          for (size_t i = 0; i < 100; i++) {
              MPI_Recv(recvarr, num_bytes, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

              // TODO: Remove the below code
//              printf ("Recv node");
//              for (size_t j = 0; j < 10; j++) {
//                  printf ("%c ", recvarr[j]);
//              }
//              printf ("\n");
          }

          elapsed_time =  MPI_Wtime() - start_time;
          bandwidth = 100 *  (num_bytes / (1024.0 * 1024.0)) / elapsed_time;
          printf ("n_bytes = %ld, time = %lf, bandwidth = %lf MBps\n", num_bytes, elapsed_time, bandwidth);
      }

      free (arr);
  }


  MPI_Finalize();
  return 0;
}
