#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define TIMES_TO_TRANSFER 100
/* 435 = 30 choose 2 - i.e. number of combinations of choosing 2 nodes from a set of 30 */
#define NUM_NODES 30
#define ENV_ENABLE_FILTER "ENABLE_FILTER"

struct Times {
    double start;
    double end;
};

struct Requests {
    MPI_Request send;
    MPI_Request recv;
};

int main( int argc, char *argv[])
{
    int myrank, size;
    char *envvar = getenv (ENV_ENABLE_FILTER);

    if (argc != 2) {
        printf ("The source file takes 1 argument - the number of bytes to send b/w processes. Exiting.\n");
        exit (-1);
    }

    int num_bytes = atoi (argv[1]);

    /* struct Requests requests[NUM_NODES][NUM_NODES];
     * struct Times times[NUM_NODES][NUM_NODES]; */
    MPI_Request recv_req[NUM_NODES];
    MPI_Request send_req[NUM_NODES];
    MPI_Status send_statuses[NUM_NODES], recv_statuses[NUM_NODES];
    struct Times times[NUM_NODES][NUM_NODES];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (size_t k = 0; k < (sizeof(bytes)/ sizeof (bytes[0])); k++) {
        char *arr, *recvarr;

        arr = malloc (sizeof (char) * num_bytes);
        recvarr = malloc (sizeof (char) * num_bytes);
        memset (arr, '0', num_bytes * sizeof(char));

        for (size_t i = 0; i < size; i++) {
            times[myrank][i].start = MPI_Wtime ();
            MPI_Isend (arr, num_bytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, &(send_req[i]));
            MPI_Irecv (recvarr, num_bytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, &(recv_req[i]));
            MPI_Wait (&(send_req[i]), &(send_statuses[i]));
            MPI_Wait (&(recv_req[i]), &(recv_statuses[i]));
            times[myrank][i].end = MPI_Wtime ();
        }

        free (arr);
        for (size_t i = 0; i < size; i++) {
            double bw  = ((double) num_bytes / (times[myrank][i].end - times[myrank][i].start)) / (1024 * 1024);
            /* We zero-out the bandwidth in the case ENV_ENABLE_FILTER env
             * variable is set. This is done so as to obtain a better heatmap */
            if (envvar != NULL && myrank == i) {
                printf ("%d %lu %f\n", myrank, i, 0.0);
            } else {
                printf ("%d %lu %f\n", myrank, i, bw);
            }
        }
    }

    MPI_Finalize();
    return 0;
}
