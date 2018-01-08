GPUMonitor
==========

This application is used to display NVIDIA GPUs' status, including device memory, GPU utilization, power and temperature. These status can also be seen in the system tray.

These useful information of GPU comes from **'nvidia-smi'** command which is opened by **popen()** function. So, it dose **NOT** support **AMD** graphics card.
