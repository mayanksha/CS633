#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <limits.h>
#include "mpi.h"
#include "vector.h"

/* #define VECTOR_DEBUG */
/* #define DEBUG
 * #define PRINT_POINTS */

typedef struct {
    int id;
    double x, y, z;
} Point;

typedef struct {
    double x, y, z;
} ClusterMean;

typedef struct {
    int         K;                      // Number of clusters
    size_t      N;                      // Total number of points
    Point       *data;
} KMeansClass;

