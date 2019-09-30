# Parallel Computing Assignment 2 - Part 2.2

## Code Description and New_Bcast Algorithm

For the `New_Bcast` function, I have used `MPI_Scatter` with `MPI_Allgather`-like ring implementation which I have implemented myself using the ring-algorithm. The `New_Bcast` function is as follows:

```c++
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
```

## Observations and Inferences

As expected, the self-implemented `New_Bcast` is slower than the native `MPI_Bcast` function. The difference in the median bandwidths for each of these functions varies based upon the `ppn` and the amount of data transferred.

For a higher ppn value (ppn = 8), we can see that the transfer speeds are much larger than for say ppn = 2. This is because intra-node transfers occur through IPC mechanisms like shared memory which are much faster than network-IO based transfers.

Also, for a higher ppn value and more number of bytes transferred (broadcasted) from root node, we see that the variance decreases and the box plots are centered around the median values for the 10 runs. Occassionally, we see that there's a large variance too (with `MPI_Bcast`). This maybe attributed to network congestion and simulatneous jobs being run by other users on the node.
