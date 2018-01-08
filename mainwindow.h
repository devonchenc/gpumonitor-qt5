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
    int getGPUInfo();
    void updateGPUInfo();
    void updateControl();
    QStringList getCommandOutput();

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
    QStringList gpuName;
    QVector<int> gpuNameLineVector;
    QVector<int> memoryTotalVector;
    QVector<int> memoryUsedVector;
    QVector<int> temperatureVector;
    QVector<float> powerDrawVector;
    QVector<float> powerLimitVector;
    QVector<int> gpuUtilVector;
    QVector<int> memoryUtilVector;

    int timerID;
};

#endif // MAINWINDOW_H
