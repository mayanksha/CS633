import matplotlib
import numpy as np
matplotlib.use('Agg')
import matplotlib.pyplot as plt

# from sklearn.datasets import make_blobs
from mst_clustering import MSTClustering

def do_clustering():

    # create some data with four clusters
    # X, y = make_blobs(200, centers=4, random_state=42)
    X = np.genfromtxt('./file16.csv', delimiter=',')
    print(X.shape)
    X = X[:,1:]

    # predict the labels with the MST algorithm
    model = MSTClustering(cutoff_scale=2)
    labels = model.fit_predict(X)

    # plot the results
    plt.scatter(X[:, 0], X[:, 1], c=labels, cmap='rainbow', marker='.')
    plt.savefig('./mst.png')

if __name__ == '__main__':
    do_clustering()
