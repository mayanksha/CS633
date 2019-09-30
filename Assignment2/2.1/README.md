# Parallel Computing Assignment 2 - Part 2.1

## Code Description and Algorithm

**The idea:** At each step, I keep a counter which takes care of the distance between the nodes which needs to communicate concurrently. In maintaining this necessary condition that if Node "i" is communicating with node "j", then no other simulatenous (concurrent) connection happens between the other nodes and these nodes "i" and "j". So, at a time, node "i" will only be communicating with node "j".

The aforementioned counter keeps track of how much distance has to be kept between a pair of ranks (between which communication needs to take place). Now, in maintaining this constraint (that one node doesn't concurrently communicate with two distinct nodes), we have to make some compromises. So, some nodes are inherently left when we're executing in this fashion because the distance causes the nodes to violate this constraint. So, I have used `MPI_Barrier` to separate such instances of execution (and these nodes are called "remaining nodes"). These "remaining nodes" are then executed later.

Also, for forcing synchronous sends, I have used `MPI_Ssend` and timed this send operation to get the transfer time for each parallel pair of transfer.
My algorithm for ensuring that each distinct pair of nodes is communicating concurrently works as follows -

```
Lets take an example on n = 6 nodes

1st Iteration:
    Nodes (0, 1) (2, 3) (4, 5) will run concurrently, each using MPI_Ssend and MPI_Recv.
    The timings for each MPI_Ssend is noted (since it's a synchronous send, so timing the Send is enough for timing the whole operation).

2nd Iteration:
    Nodes (0, 2) (1, 3) run concurrently.  Now the two remaining nodes can't be run in-parallel to these other running nodes because with wrap-around (i.e (4 + 2) mod 6 = 0), node 4 should have been communication with node 0, and node 5 to node 1. So, I use an MPI_Barrier to separate the transfers between the previous set of nodes and these two.

3rd Iteration:
    Nodes (0, 3) (1, 4) (2, 5) run concurrently.

4th Iteration:
    Nodes (0, 4) (1, 5) run concurrently. Now, nodes 2 and 3 can't communicate because (2 + 4) mod 6 = 0 which is already communicating with 4, so these communications i.e (2, 0) and (3, 1) are performed after an MPI_Barrier.

Further iterations carry on like this. At the end, we have a dense matrix of nodes who have had communicated with each other, while there are some pairs which haven't been gone through (because they violated the parallel transfer constraint).
```

## Observations and Inferences

Firstly, for communications within the same rank, the data transfer speeds are extremely high in comparison to the transfer speeds within different ranks. This is primarily because same-rank transfersuse shared memory IPC mechanisms which are extremely high in bandwidth. I have plotted two separate heatmaps - one with bandwidths for same ranks zeroed out (eg. plot-65536B.png) and one which is unfiltered (eg. plot-65536B-unfiltered.png).

Also, there are some outliers as well in some cases wherein the bandwidth is too high, but for the most part, the heatmaps show that the bandwidth is less than 100 MBps. Also, for larger data sizes, the variations between the various pairs of nodes get Also, for larger data sizes, the variations between the various pairs of nodes get Also, for larger data sizes, the bandwitdh variance between the various pairs of nodes decreases and we see a "smoother" more consitent heatmap.

There's a hump near the i = j line, where the bandwidths are consitently low. This may be attributed to the remaining nodes in the algorithm's iterations and that these nodes communicate with some overlaps (for example with n = 10, a distance of 3 means that node 6 needs to communicate with 9, and 9 needs to send to 0). This two-way transfer leads to decreased bandwidths, for these cases can't be scheduled in a completely parallel fashion (otherwise we'll have to run them sequentially).
