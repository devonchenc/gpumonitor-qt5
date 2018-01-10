#include "mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QCoreApplication>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QSize>
#include "gpuinfo.h"

MainWindow::MainWindow()
    : gpuGroupBox(Q_NULLPTR)
    , memoryLabel(Q_NULLPTR)
    , memoryEdit(Q_NULLPTR)
    , temperatureLabel(Q_NULLPTR)
    , temperatureEdit(Q_NULLPTR)
    , powerLabel(Q_NULLPTR)
    , powerEdit(Q_NULLPTR)
    , gpuUtilLabel(Q_NULLPTR)
    , gpuUtilEdit(Q_NULLPTR)
    , gpuNum(0)
{
    gpuNum = GPUInfo::getInstance()->getGPUNum();
    createIconGroupBox();

    createActions();
    createTrayIcon();

    setWindowTitle(tr("NVIDIA GPU Monitor"));
    resize(420, 400);

    if (gpuNum > 0)
    {
        updateControl();
        timerID = startTimer(1000);
    }
}

MainWindow::~MainWindow()
{
    if (gpuGroupBox)
    {
        delete [] gpuGroupBox;
    }
    if (memoryLabel)
    {
        delete [] memoryLabel;
    }
    if (memoryEdit)
    {
        delete [] memoryEdit;
    }
    if (temperatureLabel)
    {
        delete [] temperatureLabel;
    }
    if (temperatureEdit)
    {
        delete [] temperatureEdit;
    }
    if (powerLabel)
    {
        delete [] powerLabel;
    }
    if (powerEdit)
    {
        delete [] powerEdit;
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerID)
    {
        GPUInfo::getInstance()->updateInfo();

        updateControl();
    }
}

void MainWindow::updateControl()
{
    if (gpuNum == 0)
        return;

    const QVector<int>& memoryTotalVector = GPUInfo::getInstance()->getMemoryTotalVector();
    const QVector<int> memoryUsedVector = GPUInfo::getInstance()->getMemoryUsedlVector();
    const QVector<int> temperatureVector = GPUInfo::getInstance()->getTemperatureVector();
    const QVector<float> powerDrawVector = GPUInfo::getInstance()->getPowerDrawVector();
    const QVector<QString> powerLimitVector = GPUInfo::getInstance()->getPowerLimitVector();
    const QVector<int> gpuUtilVector = GPUInfo::getInstance()->getGPUUtilVector();
    const QVector<int> memoryUtilVector = GPUInfo::getInstance()->getMemoryUtilVector();

    QString toolTip;
    for (int i = 0; i < gpuNum; i++)
    {
        QString memory = QString("%1 MB / %2 MB").arg(memoryUsedVector[i]).arg(memoryTotalVector[i]);
        memoryEdit[i]->setText(memory);

        QString gpuUtil = QString("%1 %").arg(gpuUtilVector[i]);
        gpuUtilEdit[i]->setText(gpuUtil);

        QString power = QString("%1 W / %2").arg(powerDrawVector[i]).arg(powerLimitVector[i]);
        powerEdit[i]->setText(power);

        QString temperature = QString("%1 °C").arg(temperatureVector[i]);
        temperatureEdit[i]->setText(temperature);

        QString actionText = QString("GPU %1 : GPU %2%, Mem %3%, %4 °C").arg(i).
                                    arg(gpuUtilVector[i]).
                                    arg(memoryUtilVector[i]).
                                    arg(temperatureVector[i]);
        gpuInfoAction[i]->setText(actionText);
        toolTip += actionText + "\n";
    }

    trayIcon->setToolTip(toolTip);
}

void MainWindow::createIconGroupBox()
{
    if (gpuNum == 0)
        return;

    const QStringList& gpuName = GPUInfo::getInstance()->getGPUName();

    gpuGroupBox = new QGroupBox*[gpuNum];
    memoryLabel = new QLabel*[gpuNum];
    memoryEdit = new QLineEdit*[gpuNum];
    temperatureLabel = new QLabel*[gpuNum];
    temperatureEdit = new QLineEdit*[gpuNum];
    powerLabel = new QLabel*[gpuNum];
    powerEdit = new QLineEdit*[gpuNum];
    gpuUtilLabel = new QLabel*[gpuNum];
    gpuUtilEdit = new QLineEdit*[gpuNum];

    for (int i = 0; i < gpuNum; i++)
    {
        gpuGroupBox[i] = new QGroupBox(tr("GPU %1 : %2   ").arg(i).arg(gpuName[i]));

        gpuUtilLabel[i] = new QLabel(tr("GPU Util:"));
        gpuUtilEdit[i] = new QLineEdit;
        QSize size = gpuUtilEdit[i]->minimumSizeHint();
        gpuUtilEdit[i]->setMaximumWidth(size.width() + 10);
        gpuUtilEdit[i]->setFocusPolicy(Qt::NoFocus);
        memoryLabel[i] = new QLabel(tr("Memory Usage:"));
        memoryEdit[i] = new QLineEdit;
        memoryEdit[i]->setMinimumWidth(140);
        memoryEdit[i]->setMaximumWidth(140);
        memoryEdit[i]->setFocusPolicy(Qt::NoFocus);

        temperatureLabel[i] = new QLabel(tr("Temperature:"));
        temperatureEdit[i] = new QLineEdit;
        size = temperatureEdit[i]->minimumSizeHint();
        temperatureEdit[i]->setMaximumWidth(size.width() + 10);
        temperatureEdit[i]->setFocusPolicy(Qt::NoFocus);
        powerLabel[i] = new QLabel(tr("Power:"));
        powerEdit[i] = new QLineEdit;
        powerEdit[i]->setMinimumWidth(140);
        powerEdit[i]->setMaximumWidth(140);
        powerEdit[i]->setFocusPolicy(Qt::NoFocus);

        QGridLayout *infoLayout = new QGridLayout;
        infoLayout->addWidget(gpuUtilLabel[i], 0, 0, 1, 1);
        infoLayout->addWidget(gpuUtilEdit[i], 0, 1, 1, 2);
        infoLayout->addWidget(memoryLabel[i], 0, 3, 1, 1);
        infoLayout->addWidget(memoryEdit[i], 0, 4, 1, 2);

        infoLayout->addWidget(temperatureLabel[i], 1, 0, 1, 1);
        infoLayout->addWidget(temperatureEdit[i], 1, 1, 1, 2);
        infoLayout->addWidget(powerLabel[i], 1, 3, 1, 1);
        infoLayout->addWidget(powerEdit[i], 1, 4, 1, 2);

        gpuGroupBox[i]->setLayout(infoLayout);
    }

    QVBoxLayout *mainLayout = new QVBoxLayout;
    for (int i = 0; i < gpuNum; i++)
    {
        mainLayout->addWidget(gpuGroupBox[i]);
    }
    setLayout(mainLayout);
}

void MainWindow::createActions()
{
    if (gpuNum == 0)
        return;

    gpuInfoAction = new QAction*[gpuNum];

    for (int i = 0; i < gpuNum; i++)
    {
        gpuInfoAction[i] = new QAction(tr(" "), this);
    }

    restoreAction = new QAction(tr("&Restore Window"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    topmostAction = new QAction(tr("&Topmost"), this);
    topmostAction->setCheckable(true);
    connect(topmostAction, &QAction::triggered, this, &MainWindow::showTopmost);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void MainWindow::createTrayIcon()
{
    if (gpuNum == 0)
        return;

    trayIconMenu = new QMenu(this);
    for (int i = 0; i < gpuNum; i++)
    {
        trayIconMenu->addAction(gpuInfoAction[i]);
    }
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(topmostAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    QIcon icon = QIcon(":/images/nvidia.png");
    trayIcon->setIcon(icon);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayActivated);

    setWindowIcon(icon);
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        ;
    }
}

void MainWindow::showTopmost()
{
    bool checked = topmostAction->isChecked();
    if (checked)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
    }
    else
    {
        setWindowFlags(windowFlags() &~ Qt::WindowStaysOnTopHint);
    }
}
