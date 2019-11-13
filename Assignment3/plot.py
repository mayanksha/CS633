import numpy as np
import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt
import pandas as pd
import os, glob, re, sys
import seaborn as sns
import matplotlib.ticker

if (len(sys.argv) != 2):
    print ("The script accepts a folder -- 'hpc' or 'cse' in which the plot files will be generated")
    sys.exit(-1)

data_file_regex = ['./{}/data1/'.format(sys.argv[1]), './{}/data2/'.format(sys.argv[1])]

for file_regex in data_file_regex:
    if (os.path.isdir(file_regex) == 0):
        continue
    data_files = glob.glob(file_regex + 'temp_output_*')
    proc_files = sorted([(int(re.findall('temp_output_(\d+)_.*', i)[0]), i) for i in data_files])
    procs = [i[0] for i in proc_files]

    T, P, PP = [], [], []
    for a in proc_files:
        print(a)
        filename = a[1]
        f = open(filename, "r+")
        data = f.read()
        for i in re.findall('Total: (.*)', data):
            T.append([a[0], float(i)])
        for i in re.findall('Processing: (.*)', data):
            P.append([a[0], float(i)])
        for i in re.findall('Pre-Processing: (.*)', data):
            PP.append([a[0], float(i)])
        f.close()

    datas = [np.array(i) for i in [PP, P, T]]
    dfs = [pd.DataFrame(i, columns=["processes", "time"]) for i in datas]

    sns.set(style="whitegrid")
    fig, ax = plt.subplots(1, 3, figsize=(15, 10))

    fig.subplots_adjust(wspace=0.2)
    for i in range(3):
        sns.boxplot(x="processes", y="time", data=dfs[i], palette="Set3", ax=ax[i], width=0.4, showfliers=False)

    ax[0].title.set_text('Pre-Processing Time')
    ax[1].title.set_text('Processing Time')
    ax[2].title.set_text('Total Time')
    plt.savefig(file_regex + 'plot.png')

