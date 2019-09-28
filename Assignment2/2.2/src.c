#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "mpi.h"

#define TIMES_TO_TRANSFER 100
#define NUM_NODES 30
/* #define DEBUG */

/* Below function will only broadcast datatype of MPI_FLOAT to other nodes.
 * It's intentionally made non-generic because that's what the assignment
 * calls for and we just need to perform benchmarks on both of them (the
 * generic as well as the custom written Bcast).
 * */
void New_Bcast (void* d, int count, int root, MPI_Comm communicator) {
    int myrank, size;
    float *data = d;

    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    int elems_per_proc = count / size;

    float *per_proc_data = malloc (sizeof(float) * elems_per_proc);
    assert (per_proc_data != NULL);

    MPI_Scatter(data, elems_per_proc, MPI_FLOAT, per_proc_data,
                elems_per_proc, MPI_FLOAT, root, MPI_COMM_WORLD);

    for (size_t i = 0; i < elems_per_proc; i++) {
        data[myrank * elems_per_proc + i] = per_proc_data[i];
    }

    int i = 0;
    do {
        int receiving_node_rank = (myrank - 1 == -1) ? size - 1 : myrank - 1;
        int location = (myrank - i - 1 < 0 ? size + (myrank - i - 1): myrank - i - 1);
        MPI_Request send_req, recv_req;
        MPI_Status send_status, recv_status;

        MPI_Isend (&data[((location + 1)%size) * elems_per_proc], elems_per_proc, MPI_FLOAT, (myrank + 1) % size, 0, MPI_COMM_WORLD, &send_req);
        MPI_Irecv (&(data[location * elems_per_proc]), elems_per_proc, MPI_FLOAT, receiving_node_rank, 0, MPI_COMM_WORLD, &recv_req);

        MPI_Wait (&send_req, &send_status);
        MPI_Wait (&recv_req, &recv_status);

        i++;
    } while (i < size - 1);

#ifdef DEBUG
    for (size_t i = 0; i < count; i++) {
        printf ("%d, %lu, %f\n", myrank, i, data[i]);
    }
#endif
    free (per_proc_data);
}

int main( int argc, char *argv[])
{
    int myrank, size, num_elems;
    float *data = NULL;
    double t1, t2, bw1, bw2;
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
    MPI_Comm_size (MPI_COMM_WORLD, &size);

    if (argc != 2) {
        printf ("The source file takes 1 argument - the number of elems to send b/w processes. Exiting.\n");
        exit (-1);
    }

    /* The argument given is the number of bytes, so we calculate the number
     * of elements to be transferred of MPI_FLOAT datatype */
    num_elems = (atoi (argv[1])) / sizeof (float);

    data = malloc (sizeof(float) * num_elems);
    assert (data != NULL);

    for (size_t i = 0; i < num_elems; i++) {
        if (myrank == 0) {
            data[i] = (rand () / (float) RAND_MAX);
#ifdef DEBUG
            printf ("%f ", data[i]);
            if (i == (size_t)(num_elems - 1))
                printf ("\n"); 
#endif
        }
        else
            data[i] = 0.0;
    }

    t1 = MPI_Wtime();
    New_Bcast (data, num_elems, 0, MPI_COMM_WORLD);
    t1 = MPI_Wtime() - t1;

    MPI_Barrier (MPI_COMM_WORLD);

    t2 = MPI_Wtime();
    MPI_Bcast (data, num_elems, MPI_FLOAT, 0, MPI_COMM_WORLD);
    t2 = MPI_Wtime() - t2;

    bw1 = (num_elems * sizeof (float) * (size - 1)) / (t1 * 1024.0 * 1024.0);
    bw2 = (num_elems * sizeof (float) * (size - 1)) / (t2 * 1024.0 * 1024.0);

    if (myrank == 0) {
        printf ("%lf,%lf\n",bw1,bw2);
    }

    if (data != NULL) {
        free (data);
    }
    MPI_Finalize();
    return 0;
}
