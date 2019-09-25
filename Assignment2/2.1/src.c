#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

long sizes[4] = {1024, 65536, 262144, 1048576};
#define TIMES_TO_TRANSFER 100

/* 435 = 30 choose 2 - i.e. number of combinations of choosing 2 nodes from a set of 30 */
#define NUM_NODES 435
int main( int argc, char *argv[])
{
    int myrank, size;
    // MPI_Request request[];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (size_t i = 0; i < (sizeof(sizes) / sizeof(long)); i++) {
        MPI_Status status;
        double start_time, elapsed_time;
        long num_bytes = sizes[i];
        char *arr;

        arr = malloc (sizeof(char) * num_bytes);
        memset (arr, '0', num_bytes * sizeof(char));

        start_time = MPI_Wtime ();
        if (myrank == 0) {
            long long total_bytes_recvd = 0;
            double bandwidth;

            for (size_t i = 0; i < size - 1; i++) {
                for (size_t j = 0; j < TIMES_TO_TRANSFER; j++) {
                    long long curr_bytes_recvd = 0;
                    char recvarr[num_bytes];
                    MPI_Recv(recvarr, num_bytes, MPI_BYTE, MPI_ANY_SOURCE, num_bytes, MPI_COMM_WORLD, &status);

                    MPI_Get_count (&status, MPI_BYTE, (int *) &curr_bytes_recvd);
                    total_bytes_recvd += curr_bytes_recvd;
                }
            }
            elapsed_time =  MPI_Wtime() - start_time;
            bandwidth = (total_bytes_recvd / (1024.0 * 1024.0)) / elapsed_time;
            // printf ("n_bytes = %lld, time = %lf, bandwidth = %lf MBps\n", total_bytes_recvd, elapsed_time, bandwidth);
            printf ("Blocking %lf\n", bandwidth);
        } else {
            for (size_t i = 0; i < TIMES_TO_TRANSFER; i++) {
                MPI_Send(arr, num_bytes, MPI_BYTE, 0, num_bytes, MPI_COMM_WORLD);
            }
        }
        free (arr);
        sleep (1);
    }

    for (size_t i = 0; i < (sizeof(sizes) / sizeof(long)); i++) {
        double start_time, elapsed_time;
        long num_bytes = sizes[i];
        char *arr;
        MPI_Request request[TIMES_TO_TRANSFER];
        MPI_Status status[TIMES_TO_TRANSFER];

        arr = malloc (sizeof(char) * num_bytes);
        memset (arr, '0', num_bytes * sizeof(char));

        start_time = MPI_Wtime ();
        if (myrank == 0) {
            long long total_bytes_recvd = 0;
            double bandwidth;

            for (size_t i = 0; i < size - 1; i++) {
                char recvarr[num_bytes];

                for (size_t j = 0; j < TIMES_TO_TRANSFER; j++) {
                    MPI_Irecv(recvarr, num_bytes, MPI_BYTE, MPI_ANY_SOURCE, num_bytes, MPI_COMM_WORLD, &request[j]);
                }
                MPI_Waitall (TIMES_TO_TRANSFER, request, status);

                for (size_t j = 0; j < TIMES_TO_TRANSFER; j++) {
                    long long curr_bytes_recvd = 0;
                    MPI_Get_count (&status[j], MPI_BYTE, (int *) &curr_bytes_recvd);
                    total_bytes_recvd += curr_bytes_recvd;
                }
            }

            elapsed_time =  MPI_Wtime() - start_time;
            bandwidth = (total_bytes_recvd / (1024.0 * 1024.0)) / elapsed_time;
            // printf ("n_bytes = %lld, time = %lf, bandwidth = %lf MBps\n", total_bytes_recvd, elapsed_time, bandwidth);
            printf ("Non-Blocking %lf\n", bandwidth);
        } else {
            for (size_t i = 0; i < TIMES_TO_TRANSFER; i++) {
                MPI_Isend(arr, num_bytes, MPI_BYTE, 0, num_bytes, MPI_COMM_WORLD, &request[i]);
            }
            MPI_Waitall (TIMES_TO_TRANSFER, request, status);
        }
        free (arr);
        sleep (1);
    }

    MPI_Finalize();
    return 0;
}
