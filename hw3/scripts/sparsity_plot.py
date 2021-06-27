import numpy as np
import matplotlib.pyplot as plt
import os

data_path = os.path.expanduser('~/gem5/NN_out')
data = np.loadtxt(os.path.join(data_path, 'log_threshold'))
threshold = data[:, 0] / 100.0
accu = data[:, 9]

plt.plot(threshold, accu)
plt.ylabel('Accuracy')
plt.xlabel('Threshold')
plt.show()

conv_zeros = np.sum(data[:, 1:5], axis=1)
fc_zeros = np.sum(data[:, 5:9], axis=1)
plt.stackplot(threshold, conv_zeros, fc_zeros)
plt.ylabel('Number of Zeroes')
plt.xlabel('Threshold')
plt.legend(labels=['Conv', 'FC'])
plt.show()

load_time = data[:, 11] * 1e6
plt.plot(threshold, load_time)
plt.ylabel('Load Time (μs)')
plt.xlabel('Threshold')
plt.show()

conv_time = np.sum(data[:, 12:212:2], axis=1) * 1e6
fc_time = np.sum(data[:, 13:213:2], axis=1) * 1e6
plt.stackplot(threshold, conv_time, fc_time)
plt.ylabel('Inference Time (μs)')
plt.xlabel('Threshold')
plt.legend(labels=['Conv', 'FC'])
plt.show()

plt.plot(conv_zeros, conv_time, fc_zeros, fc_time, conv_zeros + fc_zeros, conv_time + fc_time)
plt.ylabel('Inference Time (μs)')
plt.xlabel('Number of Zeroes')
plt.legend(labels=['Conv', 'FC', 'Overall'])
plt.show()
