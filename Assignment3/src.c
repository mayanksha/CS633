#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mpi.h"

int main( int argc, char *argv[])
{
    int myrank, size, ret;

    if (argc != 2) {
        printf ("Error: The program takes 1 argument - File path\n.");
        return -1;
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_File fh;

    ret = MPI_File_open (MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    unsigned long long file_size, N, n_elems;
    double *data;
    MPI_Status status;
    MPI_File_get_size (fh, (MPI_Offset *) &file_size);

    N = file_size / 32;
    // printf ("File size = %llu, N = %llu\n", file_size, N);
    data = malloc (sizeof (double) * file_size);

    MPI_File_read (fh, data, file_size / sizeof (double), MPI_DOUBLE, &status);
    MPI_Get_count (&status, MPI_DOUBLE, &n_elems);
    // printf ("Actual bytes read = %llu\n", n_elems * sizeof (double));

    for (int i = 0; i < 4*N; i += 4) {
        printf ("%lld,%lf,%lf,%lf\n", (long long) data[i], data[i+1], data[i+2], data[i+3]);
    }

    free (data);
    MPI_Finalize();
    return 0;
}
