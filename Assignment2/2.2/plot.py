import numpy as np
import seaborn as sns
import matplotlib
import pandas as pd
import sys
from statistics import mean
from matplotlib import pyplot as plt

if (len(sys.argv) != 2):
    print ("The script expects a data file given as first argument! Exiting!")
    sys.exit(-1)

data_sizes = [65536, 262144, 524288, 2097152, 4194304]
ppn=[2, 4, 8]

sns.set(style="whitegrid")
a = np.genfromtxt(sys.argv[1], delimiter=',')

a_2 = a[0:50]
a_4 = a[50:100]
a_8 = a[100:150]

ppn_2 = pd.DataFrame({'DataSize': a_2[:, 0], 'ppn': a_2[:,1], 'New_Bcast': a_2[:, 2], 'MPI_Bcast': a_2[:, 3]})
ppn_4 = pd.DataFrame({'DataSize': a_4[:, 0], 'ppn': a_4[:,1], 'New_Bcast': a_4[:, 2], 'MPI_Bcast': a_4[:, 3]})
ppn_8 = pd.DataFrame({'DataSize': a_8[:, 0], 'ppn': a_8[:,1], 'New_Bcast': a_8[:, 2], 'MPI_Bcast': a_8[:, 3]})

ppn_data = [ppn_8, ppn_4, ppn_2]
plt.tight_layout()
fig, ax = plt.subplots(ncols=2, nrows=3)
fig.set_figheight(24)
fig.set_figwidth(20)

for i in range(len(ax)):
    m1 = min(np.min(ppn_data[i])[2], np.min(ppn_data[i])[3], 0)
    m2 = max(np.max(ppn_data[i])[2], np.max(ppn_data[i])[3])
    m2 += 0.08 * m2
    for j in range(len(ax[0])):
        ax[i][j].set_ylim([m1, m2])
        if (j%2 == 0):
            sns.boxplot(y='New_Bcast', x='DataSize', data=ppn_data[i], ax=ax[i][j]).set(xlabel='Data size in Bytes', ylabel='Bandwidth (MBps)')
            ax[i][j].title.set_text('Bandwidth for New_Bcast (ppn = %d)' % (ppn[i]))
        else:
            sns.boxplot(y='MPI_Bcast', x='DataSize', data=ppn_data[i], ax=ax[i][j]).set(xlabel='Data size in Bytes', ylabel='Bandwidth (MBps)')
            ax[i][j].title.set_text('Bandwidth for MPI_Bcast (ppn = %d)' % (ppn[i]))

fig.suptitle('Bandwidth vs Datasize plots for New_Bcast and MPI_Bcast for each ppn=[2, 4, 8]', size='xx-large')
filename = 'plot-' + str(int(a_2[0][-1])) + '.png'
plt.savefig(filename)
