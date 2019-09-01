# Parallel Computing Assignment 1

**Instructor** : Dr. Preeti Malakar

**Author of source code** : Mayank Sharma

## Part 1

### Question
Write a program to send D bytes from one node to another node using MPI_Send/Recv. Each send/recv should be done for 100 times. The experiment should be repeated 5 times on a pair of nodes (select nodes from your bucket only, see below).

D = {128, 1024, 65536, 1048576, 4194304} in bytes. Use 1 process per node. Plot the effective bandwidth for each data size (one plot). MBps (y-axis) and MB (x-axis).

Pseudocode:
```python
For d in D {
 Repeat 100 times
 send and recv
}
```

### [Solution]('./1.1')

## Part 2
### Question

 Write a program to send D bytes from (P-1) senders to 1 receiver (choose rank 0 as the receiver rank). Compare blocking and non-blocking sends and receives. D = {1024, 65536, 262144, 1048576} in bytes. Total #processes P = {8, 16, 32}, use 4 processes per node, i.e. #nodes = {2, 4, 8}. Sends/recvs should be repeated for 100 times. Repeat runs for each data point 5 times. Plot the effective bandwidth for each D for blocking and non-blocking for all process counts in the same graph (6 plots). MBps (y-axis) and MB (xaxis).

Pseudocode:
```python
For all process counts
For d in D {
    Repeat 100 times
        Isend/send and Irecv/recv
}
```

### [Solution]('./1.2')
