#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QDialog>

QT_BEGIN_NAMESPACE
class QAction;
class QGroupBox;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void showTopmost();

private:
    void updateControl();

    void createIconGroupBox();
    void createActions();
    void createTrayIcon();

    QGroupBox **gpuGroupBox;
    QLabel **memoryLabel;
    QLineEdit **memoryEdit;
    QLabel **temperatureLabel;
    QLineEdit **temperatureEdit;
    QLabel **powerLabel;
    QLineEdit **powerEdit;
    QLabel **gpuUtilLabel;
    QLineEdit **gpuUtilEdit;

    QAction **gpuInfoAction;
    QAction *restoreAction;
    QAction *topmostAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    int gpuNum;

    int timerID;
};

#endif // MAINWINDOW_H
