GPUMonitor-Qt5
==========

This application is written by C++/Qt5 and is used to display NVIDIA GPUs' information, including device memory usage, GPU utilization, power and temperature.

![Software interface](screenshot/GUI.png)

These status can also be seen in the system tray and right-click menu.

![SystemTray](screenshot/SystemTray.png)
![ContextMenu](screenshot/ContextMenu.png)

These useful information of GPU comes from **'nvidia-smi'** command which is opened by **popen()** function. So, it does **NOT** support **Intel** and **AMD** graphics cards.
