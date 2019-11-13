#include "src.h"

// GLOBAL VARIABLES
int myrank = -1, world_size = -1;

static inline void logger (const char* format, ...) {
#ifdef DEBUG
    va_list argp;
    va_start (argp, format);
    printf ("[%d]:\t", myrank);
    vprintf (format, argp);
    va_end (argp);
#endif
}

static inline void m_free (void *ptr) {
    if (ptr == NULL)
        return;
    free(ptr);
    ptr = NULL;
}

static inline void m_free_klass (KMeansClass *klass) {
    if (klass == NULL)
        return;
    m_free (klass->data);
    m_free (klass);
}

static inline void print_points (Point *pts, int n) {
#ifdef PRINT_POINTS
    for (int i = 0; i < n; i++)
        logger ("%d,%lf,%lf,%lf\n", pts[i].id, pts[i].x, pts[i].y, pts[i].z);
#endif
}

static void print_vectors (KMeansClass *klass, vector *c_assgn) {
    for (int i = 0; i < klass->K; i++) {
        printf ("[%d]: %d", i, c_assgn[i].total);
        /* for (int j = 0; j < 10; j++)
         *     printf ("%d ", vector_get (&c_assgn[i], j) + 1); */
        printf ("\n");
    }
}

// Squared distance between two Points.
double sqrd_dist(Point *v1, Point *v2) {
    if (v1 == NULL || v2 == NULL)
        return -1;

    double dist = 0.0;
    for (int i = 0; i < 3; i++) {
        dist += (v1->x - v2->x) * (v1->x - v2->x);
        dist += (v1->y - v2->y) * (v1->y - v2->y);
        dist += (v1->z - v2->z) * (v1->z - v2->z);
    }
    return dist;
}

int get_cluster (Point *point, Point *centroids, const int K) {
  int cluster = 0;
  double best_dist = INT_MAX;

  for (int i = 0; i < K; i++) {
    double dist = sqrd_dist(point, &(centroids[i]));
    if (dist < best_dist) {
      cluster = i;
      best_dist = dist;
    }
  }
  return cluster;
}

// Return type: Memory transfer (full)
Point *get_initial_rand_centroids (KMeansClass *klass) {
    Point *centroids = malloc (sizeof (Point) * klass->K);
    assert (centroids != NULL);

    for (int i = 0; i < klass->K; i++) {
        long rand_index = rand() % klass->N;
        /* long rand_index = i % klass->N; */
        centroids[i].x = klass->data[rand_index].x;
        centroids[i].y = klass->data[rand_index].y;
        centroids[i].z = klass->data[rand_index].z;
    }

    return centroids;
}

int main( int argc, char *argv[])
{
    int ret = EXIT_FAILURE, num_elems = 0;
    MPI_Offset file_size;
    double *data = NULL;
    int *local_clusters = NULL, *global_clusters = NULL;
    Point *local_centroids = NULL, *recv_pts = NULL;
    KMeansClass *klass = malloc (sizeof (KMeansClass));
    assert (klass != NULL);

    MPI_File fh;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    double g_time = MPI_Wtime(), pre_time = MPI_Wtime (), proc_time;

    if (argc != 3) {
        logger ("Error: The program takes 2 arguments - File path and K value\n.");
        MPI_Abort (MPI_COMM_WORLD, ret);
        return ret;
    }
    klass->K = atoi (argv[2]);
    vector c_assgn[klass->K];
    c_assgn->capacity = 0;

    ret = MPI_File_open (MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    if (ret) {
        printf ("Error while opening the file");
        goto out;
    }

    MPI_File_get_size (fh, &file_size);
    /* Because each line has 4 doubles */
    klass->N = file_size / (4 * sizeof(double));
    logger ("myrank = %d, file_size = %llu, N = %llu, K = %d\n", myrank, file_size, klass->N, klass->K);

    int pts_per_proc = klass->N / world_size;
    int remaining_pts = klass->N % world_size;
    logger ("pts_per_proc: %d, remaining_pts: %d\n", pts_per_proc, remaining_pts);

    if (myrank == 0) {
        data = malloc (sizeof (double) * file_size);
        MPI_File_read (fh, data, file_size / sizeof (double), MPI_DOUBLE, &status);

        /* Typecasting from long long * to int* may be problematic, so take care */
        MPI_Get_count (&status, MPI_DOUBLE, &num_elems);

        klass->data = malloc (sizeof (Point) * klass->N);
        for (int i = 0; i < 4 * klass->N; i+=4) {
            klass->data[i/4].x = data[i+1];
            klass->data[i/4].y = data[i+2];
            klass->data[i/4].z = data[i+3];
        }
    } else {
        klass->data = malloc (sizeof (Point) * pts_per_proc);
    }

    if (myrank == 0)
        local_centroids = get_initial_rand_centroids (klass);
    else {
        local_centroids = malloc (sizeof (Point) * klass->K);
        assert (local_centroids != NULL);
    }

    /* Allocate memory for the receive buffer. You can later change this when you'll
     * be reading from file at some stride for each process.
     *
     * Note: It only makes sense for allocating this buffer for myrank != 0 processes,
     * but MPI_Scatter doesn't allow NULL buffers. */
    recv_pts = malloc (sizeof (Point) * pts_per_proc);
    assert (recv_pts != NULL);

    /* Create a custom MPI Datatype for the Point struct we're sending */
    int count = 3;
    int array_of_blocklengths[] = { 1, 1, 1 };
    MPI_Aint array_of_displacements[] = {
        offsetof(Point, x),
        offsetof(Point, y),
        offsetof(Point, z)
    };

    MPI_Datatype array_of_types[] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE,};
    MPI_Datatype tmp_type, point_type;
    MPI_Aint lb, extent;

    MPI_Type_create_struct (count, array_of_blocklengths, array_of_displacements,
                            array_of_types, &tmp_type);
    MPI_Type_get_extent (tmp_type, &lb, &extent);
    MPI_Type_create_resized (tmp_type, lb, extent, &point_type);
    MPI_Type_commit (&point_type);
    /* */

    logger ("Starting Scatter!\n");

    MPI_Scatter (klass->data, pts_per_proc, point_type, recv_pts, pts_per_proc, point_type, 0, MPI_COMM_WORLD);
    MPI_Barrier (MPI_COMM_WORLD);

    if (myrank != 0)
        klass->data = recv_pts;

#ifdef  DEBUG
    if (myrank == 0) {
        for (int i = 1; i < world_size; i++) {
            logger ("Foo: %d,%lf,%lf,%lf\n", klass->data[i*pts_per_proc + 0].id, klass->data[i*pts_per_proc + 0].x, klass->data[i*pts_per_proc + 0].y, klass->data[i*pts_per_proc + 0].z);
            logger ("Foo: %d,%lf,%lf,%lf\n", klass->data[i*pts_per_proc + 3].id, klass->data[i*pts_per_proc + 3].x, klass->data[i*pts_per_proc + 3].y, klass->data[i*pts_per_proc + 3].z);
        }

    } else {
        logger ("Foo: %d,%lf,%lf,%lf\n", klass->data[0].id, klass->data[0].x, klass->data[0].y, klass->data[0].z);
        logger ("Foo: %d,%lf,%lf,%lf\n", klass->data[3].id, klass->data[3].x, klass->data[3].y, klass->data[3].z);
    }
#endif

    local_clusters = malloc (sizeof(int) * pts_per_proc);
    if (myrank == 0)
        global_clusters = malloc (sizeof(int) * klass->N);

    int iter = 100;

    pre_time = MPI_Wtime () - pre_time;
    proc_time = MPI_Wtime ();

    while (iter > 0) {
        /* We'll later free these set of vectors at the end of loop iteration, except
         * for the last iteration. */
        if (myrank == 0) {
            for (int i = 0; i < klass->K; i++)
                vector_init (&c_assgn[i]);
        }

        MPI_Bcast (local_centroids, klass->K, point_type, 0, MPI_COMM_WORLD);

        for (int i = 0; i < pts_per_proc; i++) {
            int cluster = get_cluster (&klass->data[i], local_centroids, klass->K);
            local_clusters[i] = cluster;
        }

        MPI_Gather (local_clusters, pts_per_proc, MPI_INT, global_clusters, pts_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

        /* Use global_clusters to re-compute the new centroids */
        for (int i = 0; i < klass->K; i++) {
            local_centroids[i].x = 0.0; local_centroids[i].y = 0.0; local_centroids[i].z = 0.0;
        }

        if (myrank == 0) {
            for (int i = 0; i < klass->N; i++) {
                // We add the actual indices of point into vectors rather than their IDs
                // Index = Actual ID - 1
                vector_add (&c_assgn[global_clusters[i]], i);
            }

            for (int i = 0; i < klass->K; i++) {
                int v_size = c_assgn[i].total;
                for (int j = 0; j < v_size; j++) {
                    int index = vector_get (&c_assgn[i], j);
                    local_centroids[i].x += klass->data[index].x;
                    local_centroids[i].y += klass->data[index].y;
                    local_centroids[i].z += klass->data[index].z;
                }
                local_centroids[i].x /= v_size;
                local_centroids[i].y /= v_size;
                local_centroids[i].z /= v_size;
            }
        }

        iter--;

        if (iter != 0 && myrank == 0)
            for (int i = 0; i < klass->K; i++)
                vector_free (&c_assgn[i]);
    }

    proc_time = MPI_Wtime () - proc_time;
    g_time = MPI_Wtime () - g_time;

    if (myrank == 0) {
        printf ("Number of Processes: %d\n", world_size);
        printf ("Total: %lf\n", g_time);
        printf ("Processing: %lf\n", proc_time);
        printf ("Pre-Processing: %lf\n", pre_time);
        printf("%d, ", klass->K);
        for (int i = 0; i < klass->K; i++) {
            printf ("(%lf,%lf,%lf)", local_centroids[i].x, local_centroids[i].y, local_centroids[i].z);
            if (i != klass->K - 1)
                printf(", ");
        }
        printf("\n");
    }
    ret = EXIT_SUCCESS;

out:
    MPI_Finalize();
    m_free (data);
    if (myrank == 0)
        for (int i = 0; i < klass->K; i++)
            vector_free (&c_assgn[i]);

    /* We free recv_pts only for ROOT process because recv_pts remains unused in that.
     * For other processes, klass->data and recv_pts both point to the same buffer. */
    if (myrank == 0)
        m_free (recv_pts);
    m_free (local_centroids);
    m_free (local_clusters);
    m_free (global_clusters);
    m_free_klass (klass);
    return ret;
}
