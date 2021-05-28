import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import os


data_path = os.path.expanduser('~/gem5/Cache_out')
ipc_o3_d = np.loadtxt(os.path.join(data_path, 'log_o3'), usecols=range(1,2)).T
ipc_minor_d = np.loadtxt(os.path.join(data_path, 'log_minor'), usecols=range(1,2)).T
ipc_o3_i = np.loadtxt(os.path.join(data_path, 'logi_o3'), usecols=range(1,2)).T
ipc_minor_i = np.loadtxt(os.path.join(data_path, 'logi_minor'), usecols=range(1,2)).T

size = 2 ** np.arange(0, 11)
plt.plot(size, ipc_o3_d, size, ipc_minor_d, size, ipc_o3_i, size, ipc_minor_i, marker='o')
plt.gca().legend(('O3 CPU L1 DCache', 'Minor CPU L1 DCache', 'O3 CPU L1 ICache', 'Minor CPU L1 ICache'))
plt.ylabel('IPC')
plt.xlabel('Cache Size (kB)')
plt.gca().set_xscale('log')
plt.gca().set_xticks(size)
plt.gca().get_xaxis().set_major_formatter(matplotlib.ticker.ScalarFormatter())
plt.show()
