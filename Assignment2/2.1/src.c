#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define TIMES_TO_TRANSFER 100
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
    MPI_Request req[2 * NUM_NODES];
    /* MPI_Status send_statuses[NUM_NODES], recv_statuses[NUM_NODES];  */
    MPI_Status statuses[2 * NUM_NODES];
    double times[NUM_NODES][NUM_NODES] = {0};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *arr, *recvarr;

    arr = malloc (sizeof (char) * num_bytes);
    recvarr = malloc (sizeof (char) * num_bytes);
    memset (arr, '0', num_bytes * sizeof(char));

    for (int i = 1; i < size ; i++) {
        MPI_Status st;
        int receiver = (myrank + i)%size;
        int receiver2 = (myrank - i) < 0? size + myrank - i: myrank - i;
        int sender = myrank;
        int k = 0;
        MPI_Request extra_req[size];
        MPI_Status extra_st[size];

        if (2*i <= size) {
            int remain = size % (2*i);
            int start_index = size - (size%(2*i));

            if (remain == 0) {
                if (myrank % (2*i) < i) {
                    printf ("Send   [%d]: %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                } else {
                    //else if (myrank % (2*i) >= i && (myrank +i)%(2*i) <  ) {
                    printf ("Recv   [%d]: %d -> %d\n", i, myrank, myrank - i);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, myrank - i, i, MPI_COMM_WORLD, &st);
                }
                } else {
                    if (myrank % (2*i) < i && myrank < start_index) {
                        printf ("*Send   [%d]: %d -> %d\n", i, myrank, receiver);
                        times[sender][receiver] = MPI_Wtime ();
                        MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                        times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                    } else if (myrank % (2*i) >= i && myrank < start_index) {
                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, myrank - i, i, MPI_COMM_WORLD, &st);
                        printf ("*recv   [%d]: %d -> %d\n", i, myrank, myrank - i);
                    }

                    MPI_Barrier (MPI_COMM_WORLD);
                    if (myrank >= start_index  && myrank < remain + start_index) {
                        printf ("**Send  [%d]: %d -> %d\n", i, myrank, receiver);
                        times[sender][receiver] = MPI_Wtime ();
                        MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                        times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                    }
                    if ((myrank >= (start_index + i)% size && myrank < i) || receiver2 >= start_index) {
                        printf ("**Recv  [%d]: %d -> %d\n", i, myrank, receiver2);
                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                    }
                }
            } else {
                int remain = 2*i - size;
                int start_index = size - i;

                if (sender < start_index) {
                    printf ("Esend [%d]: %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                } else if (myrank >= i) {
                    printf ("Erecv [%d]: %d -> %d\n", i, myrank, receiver2);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                }

                MPI_Barrier (MPI_COMM_WORLD);
                if (myrank == 3)
                    printf ("******Erecv [%d] %d -> %d, %d\n", i, myrank, receiver2, remain);
                if (myrank >= start_index && myrank < start_index + remain) {
                    printf ("*Esend [%d] %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                }
                if (myrank >= 0 && myrank < remain) {
                    printf ("*Erecv [%d] %d -> %d\n", i, myrank, receiver2);
                    // MPI_Irecv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &extra_req[k]);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                    k++;
                }
                MPI_Barrier (MPI_COMM_WORLD);

                // MPI_Waitall (k, extra_req, extra_st);
            }

            MPI_Barrier (MPI_COMM_WORLD);
        }

        MPI_Barrier (MPI_COMM_WORLD);
        MPI_Request self1, self2;
        MPI_Status st1, st2;

        times[myrank][myrank] = MPI_Wtime ();
        MPI_Isend (arr, num_bytes, MPI_BYTE, myrank, 0, MPI_COMM_WORLD, &self1);
        MPI_Irecv (recvarr, num_bytes, MPI_BYTE, myrank, 0, MPI_COMM_WORLD, &self2);
        MPI_Wait (&self1, &st1);
        MPI_Wait (&self2, &st2);
        times[myrank][myrank] = MPI_Wtime () - times[myrank][myrank];

        MPI_Barrier (MPI_COMM_WORLD);
        if (myrank != 0) {
            MPI_Send (times[myrank], size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
        } else {
            MPI_Status st;
            for (int i = 1; i < size; i++)
                MPI_Recv (times[i], size, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &st);
        }
        MPI_Bcast (times, size*size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//        if (myrank == 1) {
//            for (int i = 0; i < size; i++) {
//                for (int j = 0; j < size; j++) {
//                    printf ("%lf ", times[i][j]);
//                }
//                printf ("\n");
//            }
//        }

        if (myrank == 1)
            for (int i = 0; i < size; i++) {
                printf ("[%d] ", myrank);
                for (int j = 0; j < size; j++) {
                    printf ("%lf ", times[i][j]);
                }
                printf ("\n");
            }

        sleep (1);
        if (myrank == 0)
            for (int i = 0; i < size; i++) {
                printf ("[%d] ", myrank);
                for (int j = 0; j < size; j++) {
                    printf ("%lf ", times[i][j]);
                }
                printf ("\n");
            }
        if (myrank == 0)
        for (int i =0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (times[i][j] == 0.0f) {
                    MPI_Status st;
                    if (myrank == i || myrank == j)
                        times[i][j] = MPI_Wtime ();
                        MPI_Sendrecv (arr, num_bytes, MPI_BYTE, j, 0, recvarr, num_bytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, &st);
                        times[i][j] = MPI_Wtime () - times[myrank][myrank];
//                    if (myrank == i) {
//                        times[i][j] = MPI_Wtime ();
//                        MPI_Ssend (arr, num_bytes, MPI_BYTE, j, 0, MPI_COMM_WORLD);
//                        times[i][j] = MPI_Wtime () - times[myrank][myrank];
//                    }
//                    if (myrank == j) {
//                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, &st);
//                    }
                }
            }
        }

        printf ("My rank = %d\n", myrank);
        if (myrank != 0) {
            MPI_Send (times[myrank], size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
        } else {
            MPI_Status st;
            for (int i = 1; i < size; i++)
                MPI_Recv (times[i], size, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &st);
        }

        if (myrank == 0) {
            for (int i = 0; i < size; i++) {
                for (int j= 0; j < size; j++) {
                    double bw  = ((double) num_bytes / (times[i][j])) / (1024.0 * 1024.0);
                    /* We zero-out the bandwidth in the case ENV_ENABLE_FILTER env
                     * variable is set. This is done so as to obtain a better heatmap */
                    if (envvar != NULL) {
                        printf ("%d %d %lf\n", i, j, 0.0);
                    } else {
                        printf ("%d %d %lf\n", i, j, bw);
                    }
                }
            }
        }

        free (arr);
        free (recvarr);
        MPI_Finalize();
        return 0;
    }
