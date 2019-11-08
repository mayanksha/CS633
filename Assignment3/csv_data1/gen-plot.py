import numpy as np
import matplotlib
matplotlib.use('Agg')

from mpl_toolkits.mplot3d import Axes3D
from matplotlib import pyplot as plt

ncols=4
#  fig = plt.figure()
#  ax = fig.add_subplot(111, projection='3d')

fig, ax = plt.subplots(nrows=1, ncols=ncols, figsize=(12,4), subplot_kw=dict(projection='3d'))
for i in range(0, ncols):
    a = np.genfromtxt('./file{}.csv'.format((i*(17 // ncols)) % 17), delimiter=',')
    x = a[:,1]
    y = a[:,2]
    z = a[:,3]
    ax[i].scatter(x, y, z, c='r', marker='.')
    ax[i].set_title('file{}.csv'.format((i * (17 // ncols)) % 17))

for i in ax:
    i.set_ylim(-10, 10)
    i.set_xlim(-20, 20)
    i.set_zlim(-15, 15)

plt.savefig('plt.png')
