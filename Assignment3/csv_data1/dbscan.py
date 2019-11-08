import numpy as np
import pandas as pd
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

from mpl_toolkits.mplot3d import Axes3D
from sklearn.cluster import DBSCAN
from sklearn import metrics
from sklearn.datasets.samples_generator import make_blobs
from sklearn.preprocessing import StandardScaler

X = np.genfromtxt('./file10.csv', delimiter=',')
df = pd.DataFrame(X, columns=['ID', 'x','y','z'])

ids = X[:,0]
X = X[:,1:]
X = StandardScaler().fit_transform(X)

db = DBSCAN(n_jobs=-1).fit(X)
core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
core_samples_mask[db.core_sample_indices_] = True
labels = db.labels_

print(labels.shape)
print(set(labels))

# Number of clusters in labels, ignoring noise if present.
n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
n_noise_ = list(labels).count(-1)

print('Estimated number of clusters: %d' % n_clusters_)
print('Estimated number of noise points: %d' % n_noise_)
# print("Homogeneity: %0.3f" % metrics.homogeneity_score(labels_true, labels))
# print("Completeness: %0.3f" % metrics.completeness_score(labels_true, labels))
# print("V-measure: %0.3f" % metrics.v_measure_score(labels_true, labels))
# print("Adjusted Rand Index: %0.3f" % metrics.adjusted_rand_score(labels_true, labels))

# Black removed and is used for noise instead.
unique_labels = set(labels)
colors = [plt.cm.Spectral(each)
          for each in np.linspace(0, 1, len(unique_labels))]

clusters = [X[labels == i] for i in range(n_clusters_)]
outliers = X[labels == -1]

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
#  ax.scatter(np.array(df['x']),np.array(df['y']),np.array(df['z']), marker="s", c=df["Cluster"], s=40, cmap="RdBu")
#  ax.scatter(X[:,0], X[:,1], X[:,2], marker="s", c=df["Cluster"], s=40, cmap="RdBu")

plt.show()
#  plt.savefig('./dbscan.png')
