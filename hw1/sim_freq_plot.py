import numpy as np
import matplotlib.pyplot as plt
import os


data_path = os.path.expanduser('~/gem5/Frequency_out')
data_simple = np.loadtxt(os.path.join(data_path, 'log_TimingSimpleCPU'))
data_minor = np.loadtxt(os.path.join(data_path, 'log_MinorCPU'))

freq_simple, sim_seconds_simple = data_simple.T
freq_minor, sim_seconds_minor = data_minor.T
plt.plot(freq_simple, sim_seconds_simple, freq_minor, sim_seconds_minor, marker='o')
plt.gca().legend(('Timing Simple CPU', 'Minor CPU'))
plt.xlabel('Frequency (MHz)')
plt.ylabel('Simulated Time (s)')
plt.show()
