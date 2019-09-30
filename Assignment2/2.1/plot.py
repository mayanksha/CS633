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
import re
matplotlib.rc('figure', figsize=(13, 7))

if (len(sys.argv) != 3):
    print("The script accepts two positional arguments - The name for data file and the name with which the plot has to be saved. ")
    sys.exit (-1);

file = open(sys.argv[1], "r")
X, Y, Z = [], [], [];

for l in file:
    row = l.split()
    X.append(float(row[0].lstrip().rstrip()))
    Y.append(float(row[1].lstrip().rstrip()))
    Z.append(float(row[2].lstrip().rstrip()))

file.close();

data = pd.DataFrame({'X': X, 'Y': Y, 'Z': Z})
data_pivoted = data.pivot("Y", "X", "Z")
ax = sns.heatmap(data_pivoted, cbar_kws={'label': 'Bandwidth (in MBps)'})
bytes_transferred = re.findall(r"\d+", sys.argv[1])[0]
plt.title("Heatmap for Concurrent communications b/w 30 nodes sending %d bytes each" % (int(bytes_transferred)))
filename = sys.argv[2]
plt.savefig(filename)
print("Plot saved as %s" % (filename))
# plt.show()
