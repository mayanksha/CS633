import sys
import numpy as np
import multiprocessing
import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt

from sklearn.metrics import silhouette_score
from sklearn.cluster import MiniBatchKMeans

if (len(sys.argv) != 2):
    print("One positional argument is required -- a single integer $A so that file${A}.csv will be used as dataset.")
    sys.exit(-1)

# Constants
BATCH_SIZE = 30
MAX_K_VALUE = int(2.5 * BATCH_SIZE)

# Read the data
X = np.genfromtxt('./file%d.csv' % (int(sys.argv[1])), delimiter=',')
n_rows = int(0.4 * X.shape[0])
X = X[np.random.choice(X.shape[0], n_rows, replace=False)]
X = X[:,1:]

def get_slope(a, b):
    return (a[1] - b[1]) / (a[0] - b[0])

def bin_search_k(k_arr, sqr_cost):
    n = len(k_arr)
    points = [[k_arr[i], sqr_cost[i]] for i in range(n)]

    slopes = []
    for i in range(n-1):
        slopes.append(get_slope(points[i], points[i+1]))

    slopes = np.array(slopes)
    granularity = 5
    std_devs = []
    for i in range(granularity, len(slopes)+1, granularity):
        arr = slopes[i-granularity:i]
        std_devs.append((i-granularity, np.var(arr) + abs(arr[0] - arr[-1])))
    #  print(std_devs)

def kmeans(k):
    global r
    KM = MiniBatchKMeans(n_clusters = k, max_iter=200, batch_size=BATCH_SIZE)
    KM.fit(X)

    print(k)
    labels = KM.labels_
    # calculates squared error
    # for the clustered points
    return k, KM.inertia_, silhouette_score(X, labels, metric = 'euclidean')

pool = multiprocessing.Pool(processes=10)
r = range(2, MAX_K_VALUE, 10)
k, cost, sil = zip(*pool.map(kmeans, r))

k, cost, sil = list(k), list(cost), list(sil)
bin_search_k(k, cost)
# plot the cost against K values
fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(12, 12))
ax[0].plot(k, cost, color ='g', linewidth ='1')
ax[0].scatter(k, cost, color='r')
ax[0].set_xlabel("Value of K")
ax[0].set_ylabel("Sqaured Error (Cost)")

ax[1].plot(k, sil, color ='g', linewidth ='1')
ax[1].scatter(k, sil, color='r')

print(k[np.argmax(sil[np.argmin(sil):200])])
plt.savefig('./kmeans-%d.png' % (int(sys.argv[1])))
#  plt.show() # clear the plot
