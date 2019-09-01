import numpy as np
import matplotlib
from statistics import mean

matplotlib.use('Agg')
from matplotlib import pyplot as plt

def set_box_color(bp, color):
    plt.setp(bp['boxes'], color=color)
    plt.setp(bp['whiskers'], color=color)
    plt.setp(bp['caps'], color=color)
    plt.setp(bp['medians'], color=color)

nodes = [2, 4, 8]
blocking_bw = []
non_blocking_bw = []
bytes_transferred = [102400, 6553600, 26214400, 104857600]
colors = ['red', 'green', 'blue']
ticks = [i+1 for i in range(len(bytes_transferred))]
xtick_names = [round((i/(1024)**2), 4) for i in bytes_transferred]

for i in nodes:
    temp1 = []
    temp2 = []
    for j in range(1, 6):
        bandwidth_values = np.genfromtxt('./data-%s-%s.txt' % (str(i), str(j))).T[1]
        temp1 += list(bandwidth_values[0:4])
        temp2 += list(bandwidth_values[4:])
    blocking_bw += [temp1]
    non_blocking_bw += [temp2]

fig1, ax1 = plt.subplots(figsize=(12,12))
for i in range(len(blocking_bw)):
    data = np.array(blocking_bw[i]).reshape((4, 5)).T
    ax1.set_title('Bandwidth (MBps) vs Bytes transferred for Blocking calls (%s nodes)' % (nodes))
    plt.ylabel('Bandwidth (in MBps)')
    plt.xlabel('Data Transferred (in MegaBytes)')
    bp = ax1.boxplot(data, showfliers=True)
    plt.setp(bp['whiskers'], color='k', linestyle='--')
    set_box_color(bp, colors[i])

    x_median = []
    y_median = []
    for j in range(len(bytes_transferred)):
        x_median.append(mean(bp['medians'][j].get_xdata()))
        y_median.append(mean(bp['medians'][j].get_ydata()))
    ax1.plot(x_median, y_median, color=colors[i], marker='.', linestyle='-', linewidth=2, label=str(nodes[i]) + " Nodes")

ax1.legend()
plt.xticks(ticks, xtick_names)
for tick in ax1.xaxis.get_major_ticks():
    tick.label.set_fontsize(12)

fig2, ax2 = plt.subplots(figsize=(12,12))
for i in range(len(non_blocking_bw)):
    data = np.array(non_blocking_bw[i]).reshape((4, 5)).T
    ax2.set_title('Bandwidth (MBps) vs Bytes transferred for Non-Blocking calls (%s nodes)' % (nodes))
    plt.ylabel('Bandwidth (in MBps)')
    plt.xlabel('Data Transferred (in MegaBytes)')
    bp = ax2.boxplot(data, showfliers=True)
    plt.setp(bp['whiskers'], color='k', linestyle='--')
    set_box_color(bp, colors[i])

    x_median = []
    y_median = []
    for j in range(len(bytes_transferred)):
        x_median.append(mean(bp['medians'][j].get_xdata()))
        y_median.append(mean(bp['medians'][j].get_ydata()))
    ax2.plot(x_median, y_median, color=colors[i], marker='.', linestyle='-', linewidth=2, label=str(nodes[i]) + " Nodes")

ax2.legend()
plt.xticks(ticks, xtick_names)
for tick in ax2.xaxis.get_major_ticks():
    tick.label.set_fontsize(12)

fig, ax = plt.subplots(ncols=2, figsize=(12,12))
for i in range(len(blocking_bw)):
    data = np.array(blocking_bw[i]).reshape((4, 5)).T
    ax[0].set_title('Bandwidth vs Bytes transferred for Blocking calls')
    ax[0].set_ylabel('Bandwidth (in MBps)')
    ax[0].set_xlabel('Data Transferred (in MegaBytes)')
    bp = ax[0].boxplot(data, showfliers=True)
    plt.setp(bp['whiskers'], color='k', linestyle='--')
    set_box_color(bp, colors[i])

    x_median = []
    y_median = []
    for j in range(len(bytes_transferred)):
        x_median.append(mean(bp['medians'][j].get_xdata()))
        y_median.append(mean(bp['medians'][j].get_ydata()))
    ax[0].plot(x_median, y_median, color=colors[i], marker='.', linestyle='-', linewidth=2, label=str(nodes[i]) + " Nodes")

ax[0].legend()
ax[0].set_ylim([0, 250])
ax[0].set_xticks(ticks)
ax[0].set_xticklabels(xtick_names)

for tick in ax[0].xaxis.get_major_ticks():
    tick.label.set_fontsize(12)

for i in range(len(non_blocking_bw)):
    data = np.array(non_blocking_bw[i]).reshape((4, 5)).T
    ax[1].set_title('Bandwidth vs Bytes transferred for Non-Blocking calls')
    ax[1].set_ylabel('Bandwidth (in MBps)')
    ax[1].set_xlabel('Data Transferred (in MegaBytes)')
    bp = ax[1].boxplot(data, showfliers=True)
    plt.setp(bp['whiskers'], color='k', linestyle='--')
    set_box_color(bp, colors[i])

    x_median = []
    y_median = []
    for j in range(len(bytes_transferred)):
        x_median.append(mean(bp['medians'][j].get_xdata()))
        y_median.append(mean(bp['medians'][j].get_ydata()))
    ax[1].plot(x_median, y_median, color=colors[i], marker='.', linestyle='-', linewidth=2, label=str(nodes[i]) + " Nodes")

ax[1].legend()
ax[1].set_ylim([0, 250])
ax[1].set_xticks(ticks)
ax[1].set_xticklabels(xtick_names)
for tick in ax[1].xaxis.get_major_ticks():
    tick.label.set_fontsize(12)

fig1.savefig('./blocking-plot.jpg')
fig2.savefig('./non-blocking-plot.jpg')
fig.savefig('./plot.jpg')

plt.show()
