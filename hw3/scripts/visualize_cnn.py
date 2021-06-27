import numpy as np
import torch
from torch import nn
from captum.attr import visualization as viz
import matplotlib.pyplot as plt

model_size = [5*5*1*32, 32, 5*5*32*64, 64, 3136*512, 512, 512*10, 10]
model_data = np.fromfile('ModelData', dtype=np.float32)

img = np.fromfile('t10k-images.idx3-ubyte', dtype=np.uint8)
img = img[16:16+784]
viz.visualize_image_attr(img.reshape(28, 28, 1),
                         method="heat_map",
                         sign="all",
                         show_colorbar=False,
                         use_pyplot=True)

img = torch.from_numpy(img.reshape(1, 1, 28, 28)).float() / 255

offset = 0
cnt = 0

conv2d_kernel = model_data[offset:offset+model_size[cnt]].reshape(5, 5, 1, 32)
offset = offset + model_size[cnt]
cnt = cnt + 1

print(np.mean(conv2d_kernel))
print(np.std(conv2d_kernel))

fig, axs = plt.subplots(1, 32, figsize=(7 * 32, 6))
for i in range(32):
    viz.visualize_image_attr(conv2d_kernel[:, :, 0, i].reshape(5, 5, 1),
                             method="heat_map",
                             sign="all",
                             show_colorbar=False,
                             plt_fig_axis=(fig, axs[i]),
                             use_pyplot=False)
plt.savefig('conv2d_kernel.png')
plt.show()

conv2d_bias = model_data[offset:offset+model_size[cnt]].reshape(32)
offset = offset + model_size[cnt]
cnt = cnt + 1

conv2d = nn.Conv2d(1, 32, (5, 5), padding=2)
pool = nn.MaxPool2d(2, stride=2)
conv2d.weight.data = torch.tensor(np.moveaxis(np.moveaxis(conv2d_kernel, -1, 0), -1, 1))
conv2d.bias.data = torch.tensor(conv2d_bias)
conv2d_output = pool(conv2d(img))
conv2d_output_show = conv2d_output.detach().numpy()
fig, axs = plt.subplots(1, 32, figsize=(7 * 32, 6))
for i in range(32):
    viz.visualize_image_attr(conv2d_output_show[0, i, :, :].reshape(14, 14, 1),
                             method="heat_map",
                             sign="all",
                             show_colorbar=False,
                             plt_fig_axis=(fig, axs[i]),
                             use_pyplot=False)
plt.show()

conv2d1_kernel = model_data[offset:offset+model_size[cnt]].reshape(5, 5, 32, 64)
offset = offset + model_size[cnt]
cnt = cnt + 1

print(np.mean(conv2d1_kernel))
print(np.std(conv2d1_kernel))

fig, axs = plt.subplots(32, 64, figsize=(6 * 64, 6 * 32))
for i in range(32):
    for j in range(64):
        viz.visualize_image_attr(conv2d1_kernel[:, :, i, j].reshape(5, 5, 1),
                                 method="heat_map",
                                 sign="all",
                                 show_colorbar=False,
                                 plt_fig_axis=(fig, axs[i, j]),
                                 use_pyplot=False)
plt.savefig('conv2d1_kernel.png')
plt.show()

conv2d1_bias = model_data[offset:offset+model_size[cnt]].reshape(64)
offset = offset + model_size[cnt]
cnt = cnt + 1

conv2d1 = nn.Conv2d(32, 64, (5, 5), padding=2)
conv2d1.weight.data = torch.tensor(np.moveaxis(np.moveaxis(conv2d1_kernel, -1, 0), -1, 1))
conv2d1.bias.data = torch.tensor(conv2d1_bias)
conv2d1_output = pool(conv2d1(conv2d_output))
conv2d1_output_show = conv2d1_output.detach().numpy()
fig, axs = plt.subplots(1, 64, figsize=(6 * 64, 6))
for i in range(64):
    viz.visualize_image_attr(conv2d1_output_show[0, i, :, :].reshape(7, 7, 1),
                             method="heat_map",
                             sign="all",
                             show_colorbar=False,
                             plt_fig_axis=(fig, axs[i]),
                             use_pyplot=False)
plt.savefig('conv2d1_ouput.png')
plt.show()

dense_kernel = model_data[offset:offset+model_size[cnt]].reshape(3136, 512, 1)
offset = offset + model_size[cnt]
cnt = cnt + 1

print(np.mean(dense_kernel))
print(np.std(dense_kernel))

viz.visualize_image_attr(dense_kernel,
                         method="heat_map",
                         sign="all",
                         show_colorbar=False,
                         fig_size=(10, 60),
                         use_pyplot=True)

dense_bias = model_data[offset:offset+model_size[cnt]].reshape(512)
offset = offset + model_size[cnt]
cnt = cnt + 1

dense = nn.Linear(3136, 512)
dense.weight.data = torch.tensor(dense_kernel.swapaxes(0, 1).reshape(512, 3136))
dense.bias.data = torch.tensor(dense_bias)
dense_output = dense(torch.movedim(conv2d1_output, 1, 3).reshape(3136))
dense_output_show = dense_output.detach().numpy()
viz.visualize_image_attr(dense_output_show.reshape(16, 32, 1),
                         method="heat_map",
                         sign="all",
                         show_colorbar=False,
                         fig_size=(6, 3),
                         use_pyplot=True)

dense1_kernel = model_data[offset:offset+model_size[cnt]].reshape(512, 10, 1)
offset = offset + model_size[cnt]
cnt = cnt + 1

print(np.mean(dense1_kernel))
print(np.std(dense1_kernel))

viz.visualize_image_attr(dense1_kernel,
                         method="heat_map",
                         sign="all",
                         show_colorbar=False,
                         fig_size=(1, 10),
                         use_pyplot=True)

dense1_bias = model_data[offset:offset+model_size[cnt]].reshape(10)
offset = offset + model_size[cnt]
cnt = cnt + 1

dense1 = nn.Linear(512, 10)
dense1.weight.data = torch.tensor(dense1_kernel.swapaxes(0, 1).reshape(10, 512))
dense1.bias.data = torch.tensor(dense1_bias)
dense1_output = dense1(dense_output.view(512))
dense1_output_show = dense1_output.detach().numpy()
viz.visualize_image_attr(dense1_output_show.reshape(1, 10, 1),
                         method="heat_map",
                         sign="positive",
                         show_colorbar=False,
                         fig_size=(6, 1),
                         use_pyplot=True)
