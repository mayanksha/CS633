# -*- coding: utf-8 -*-
"""
Created on Thu Aug 15 20:48:21 2019

@author: Sanket
"""

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import matplotlib
import sys

matplotlib.rc('figure', figsize=(13, 7))


file = open(sys.argv[1], "r")
print(sys.argv[1])
X, Y, Z = [], [], [];

for l in file:
    row = l.split()
    X.append(float(row[0].lstrip().rstrip()))
    Y.append(float(row[1].lstrip().rstrip()))
    Z.append(float(row[2].lstrip().rstrip()))

file.close();

data = pd.DataFrame({'X': X, 'Y': Y, 'Z': Z})
data_pivoted = data.pivot("Y", "X", "Z")
ax = sns.heatmap(data_pivoted)
plt.title(sys.argv[1])
#  plt.savefig("plot.png")
plt.show()

