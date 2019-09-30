#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

#define TIMES_TO_TRANSFER 100
#define NUM_NODES 30
#define ENV_ENABLE_FILTER "ENABLE_FILTER"

typedef enum { false, true } bool;
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
    MPI_Status st;

    if (argc != 2) {
        printf ("The source file takes 1 argument - the number of bytes to send b/w processes. Exiting.\n");
        exit (-1);
    }

    int num_bytes = atoi (argv[1]);

    /* struct Requests requests[NUM_NODES][NUM_NODES];
     * struct Times times[NUM_NODES][NUM_NODES]; */
    /* MPI_Status send_statuses[NUM_NODES], recv_statuses[NUM_NODES];  */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    char *arr, *recvarr;

    double times[size][size];
    for (int i =0; i < size; i++)
        for (int j = 0; j < size; j++)
            times[i][j] = 0.0f;
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
                    dprintf (STDERR_FILENO, "Send   [%d]: %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                } else {
                    dprintf (STDERR_FILENO, "Recv   [%d]: %d -> %d\n", i, myrank, myrank - i);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, myrank - i, i, MPI_COMM_WORLD, &st);
                }
                } else {
                    if (myrank % (2*i) < i && myrank < start_index) {
                        dprintf (STDERR_FILENO, "*Send   [%d]: %d -> %d\n", i, myrank, receiver);
                        times[sender][receiver] = MPI_Wtime ();
                        MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                        times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                    } else if (myrank % (2*i) >= i && myrank < start_index) {
                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, myrank - i, i, MPI_COMM_WORLD, &st);
                        dprintf (STDERR_FILENO, "*recv   [%d]: %d -> %d\n", i, myrank, myrank - i);
                    }

                    MPI_Barrier (MPI_COMM_WORLD);
                    if (myrank >= start_index  && myrank < remain + start_index) {
                        dprintf (STDERR_FILENO, "**Send  [%d]: %d -> %d\n", i, myrank, receiver);
                        times[sender][receiver] = MPI_Wtime ();
                        MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                        times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                    }
                    if ((myrank >= (start_index + i)% size && myrank < i) || receiver2 >= start_index) {
                        dprintf (STDERR_FILENO, "**Recv  [%d]: %d -> %d\n", i, myrank, receiver2);
                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                    }
                }
            } else {
                int remain = 2*i - size;
                int start_index = size - i;

                if (sender < start_index) {
                    dprintf (STDERR_FILENO, "Esend [%d]: %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                } else if (myrank >= i) {
                    dprintf (STDERR_FILENO, "Erecv [%d]: %d -> %d\n", i, myrank, receiver2);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                }

                MPI_Barrier (MPI_COMM_WORLD);
                if (myrank == 3)
                    dprintf (STDERR_FILENO, "******Erecv [%d] %d -> %d, %d\n", i, myrank, receiver2, remain);
                if (myrank >= start_index && myrank < start_index + remain) {
                    dprintf (STDERR_FILENO, "*Esend [%d] %d -> %d\n", i, myrank, receiver);
                    times[sender][receiver] = MPI_Wtime ();
                    MPI_Ssend (arr, num_bytes, MPI_BYTE, receiver, i, MPI_COMM_WORLD);
                    times[sender][receiver] = MPI_Wtime () - times[sender][receiver];
                }
                if (myrank >= 0 && myrank < remain) {
                    dprintf (STDERR_FILENO, "*Erecv [%d] %d -> %d\n", i, myrank, receiver2);
                    MPI_Recv (recvarr, num_bytes, MPI_BYTE, receiver2, i, MPI_COMM_WORLD, &st);
                    k++;
                }
                MPI_Barrier (MPI_COMM_WORLD);
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

        MPI_Barrier (MPI_COMM_WORLD);
        if (myrank == 0) {
            MPI_Request req[size-1];
            MPI_Status statuses[size-1];
            for (int i = 1; i < size; i++)
                MPI_Isend (times, size*size, MPI_DOUBLE, i, 100, MPI_COMM_WORLD, &(req[i-1]));
            MPI_Waitall (size-1, req, statuses);
        } else {
            MPI_Recv (times, size * size, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD, &st);
        }

        MPI_Barrier (MPI_COMM_WORLD);

        bool loc[size][size];
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                loc[i][j] = false;

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (times[i][j] == 0.0f) {
                    loc[i][j] = true;
                    if (myrank == i) {
                        times[i][j] = MPI_Wtime ();
                        MPI_Ssend (arr, num_bytes, MPI_BYTE, j, 0, MPI_COMM_WORLD);
                        times[i][j] = MPI_Wtime () - times[i][j];
                    }
                    if (myrank == j) {
                        MPI_Recv (recvarr, num_bytes, MPI_BYTE, i, 0, MPI_COMM_WORLD, &st);
                    }
                }
            }
        }

        int count = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (loc[i][j]) {
                    count++;
                    MPI_Request root_req;
                    MPI_Status root_status;
                    double transfer_data[3] = {(float)i, (float)j, times[i][j]};
                    if (myrank == i) {
                        MPI_Send (transfer_data, 3, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
                    }
                    if (myrank == 0) {
                        MPI_Irecv (transfer_data, 3, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &root_req);
                        MPI_Wait (&root_req, &root_status);
                        times[(int)transfer_data[0]][(int)transfer_data[1]] = transfer_data[2];
                    }
                }
            }
        }

        MPI_Request root_req[count];
        MPI_Status root_st[count];

//        if (myrank == 0)
//            for (int i = 0; i < size; i++) {
//                printf ("[%d] ", i);
//                for (int j = 0; j < size; j++) {
//                    printf ("%lf ", times[i][j]);
//                }
//                printf ("\n");
//            }

        if (myrank != 0) {
            MPI_Send (times[myrank], size, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
        } else {
            for (int i = 1; i < size; i++)
                MPI_Recv (times[i], size, MPI_DOUBLE, i, 99, MPI_COMM_WORLD, &st);
        }

        if (myrank == 0) {
            for (int i = 0; i < size; i++) {
                for (int j= 0; j < size; j++) {
                    double bw  = ((double) num_bytes / (times[i][j])) / (1024.0 * 1024.0);
                    /* We zero-out the bandwidth in the case ENV_ENABLE_FILTER env
                     * variable is set. This is done so as to obtain a better heatmap */
                    if (i == j) {
                        if (envvar != NULL) {
                            printf ("%d %d %lf\n", i, j, 0.0);
                        } else {
                            printf ("%d %d %lf\n", i, j, bw);
                        }
                    } else
                        printf ("%d %d %lf\n", i, j, bw);
                }
            }
        }

        free (arr);
        free (recvarr);
        MPI_Finalize();
        return 0;
    }
