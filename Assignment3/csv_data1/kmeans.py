from time import time
import numpy as np
import multiprocessing
import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt

from sklearn.metrics import silhouette_score
from sklearn.cluster import KMeans
from sklearn.datasets import load_digits
from sklearn.decomposition import PCA
from sklearn.preprocessing import scale

X = np.genfromtxt('./file16.csv', delimiter=',')

fig, ax = plt.subplots(nrows=1, ncols=2, figsize=(12, 12))

r = range(2, 100, 5)
def kmeans(k):
    global r
    KM = KMeans(n_clusters = k)
    KM.fit(X[:,1:])

    print(k)
    labels = KM.labels_
    # calculates squared error
    # for the clustered points
    return k, KM.inertia_, silhouette_score(X[:,1:], labels, metric = 'euclidean')

pool = multiprocessing.Pool(processes=8)
k, cost, sil = zip(*pool.map(kmeans, r))

# plot the cost against K values
ax[0].plot(k, cost, color ='g', linewidth ='1')
ax[0].scatter(k, cost, color='r')
ax[0].set_xlabel("Value of K")
ax[0].set_ylabel("Sqaured Error (Cost)")

ax[1].plot(k, sil, color ='g', linewidth ='1')
ax[1].scatter(k, sil, color='r')

plt.savefig('./kmeans-k.png')
# plt.show() # clear the plot
