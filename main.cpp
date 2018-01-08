#include "mainwindow.h"
#include <QApplication>

#include <QMessageBox>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0, QObject::tr("GPUMonitor"),
                              QObject::tr("Couldn't detect any system tray on this system."));
        return 1;
    }

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
