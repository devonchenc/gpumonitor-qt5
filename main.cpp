#include "mainwindow.h"
#include <QApplication>

#include <QMessageBox>
#include <QSharedMemory>
#include "gpuinfo.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if (GPUInfo::getInstance()->getGPUNum() == 0)
    {
        QMessageBox::critical(nullptr, QObject::tr("GPUMonitor"),
                              QObject::tr("Couldn't detect any NVIDIA GPU on this system."));
        return 1;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(nullptr, QObject::tr("GPUMonitor"),
                              QObject::tr("Couldn't detect any system tray on this system."));
        return 1;
    }

    // Make sure only run one application instance
    QSharedMemory shared("GPUMonitor");
    if (shared.attach())
    {
        return 0;
    }
    shared.create(1);

    QApplication::setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    return a.exec();
}
