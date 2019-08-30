import numpy as np
import matplotlib

matplotlib.use('Agg')
from matplotlib import pyplot as plt

ticks = [i+1 for i in range(5)]
bytes_transferred = [128, 1024, 65536, 1048576, 4194304]
xtick_names = [str(round(i/ (1024)**2, 6)) for i in bytes_transferred]

data = np.loadtxt('./data.txt').reshape((5,5))
print (data)

fig, ax = plt.subplots()
plt.boxplot(data)
plt.ylabel('Bandwidth (in MBps)')
plt.xlabel('Data Transferred (in MegaBytes)')
plt.xticks(ticks, xtick_names)
ax.set_ylim(0, 120)
plt.savefig('./plot.png')
