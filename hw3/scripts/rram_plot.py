import numpy as np
import matplotlib.pyplot as plt
import os

data_path = os.path.expanduser('~/gem5/NN_out')
data = np.loadtxt(os.path.join(data_path, 'log_noise'))
data_load_once = np.loadtxt(os.path.join(data_path, 'log_noise_load_once'))
noise = data[:, 0]
accu = data[:, 1]
accu_load_once = data_load_once[:, 1]

plt.plot(noise, accu, noise, accu_load_once)
plt.ylabel('Accuracy')
plt.xlabel('Noise standard deviation')
plt.legend(labels=['Load multiple times', 'Load once'])
plt.show()
