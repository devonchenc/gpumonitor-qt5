#include "mainwindow.h"

#include <QAction>
#include <QMenu>
#include <QCoreApplication>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QSize>

#include <vector>

MainWindow::MainWindow()
    : gpuGroupBox(Q_NULLPTR)
    , nameLabel(Q_NULLPTR)
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
    getGPUInfo();
    createIconGroupBox();

    createActions();
    createTrayIcon();

    setWindowTitle(tr("NVIDIA GPU Monitor"));
    resize(500, 150);

    updateGPUInfo();
    updateControl();
    timerID = startTimer(1000);
}

MainWindow::~MainWindow()
{
    if (gpuGroupBox)
    {
        delete [] gpuGroupBox;
    }
    if (nameLabel)
    {
        delete [] nameLabel;
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
    if(event->timerId() == timerID)
    {
        updateGPUInfo();

        updateControl();
    }
}

int MainWindow::getGPUInfo()
{
    QStringList strVector = getCommandOutput();

    for (int i = 0; i < strVector.size(); i++)
    {
        QString str = strVector[i];
        int pos = str.indexOf("Attached GPUs");
        if (pos != -1)
        {
            pos = str.indexOf(":");
            gpuNum = str.mid(pos + 2).toInt();
        }
        pos = str.indexOf("Product Name");
        if (pos != -1)
        {
            pos = str.indexOf(":");
            gpuName.append(str.mid(pos + 2));
            gpuNameLineVector.append(i);
        }
    }

    if (gpuNum > 0)
    {
        memoryTotalVector.resize(gpuNum);
        memoryUsedVector.resize(gpuNum);
        temperatureVector.resize(gpuNum);
        powerDrawVector.resize(gpuNum);
        powerLimitVector.resize(gpuNum);
        memoryUtilVector.resize(gpuNum);
        gpuUtilVector.resize(gpuNum);
    }

    return gpuNum;
}

void MainWindow::updateGPUInfo()
{
    if (gpuNum < 1)
        return;

    QStringList strVector = getCommandOutput();

    int memoryLine, temperatureLine, powerLine, utilizationLine;
    for (int n = 0; n < gpuNum; n++)
    {
        int upperLimit;
        if (n < gpuNum - 1)
            upperLimit = gpuNameLineVector[n + 1];
        else
            upperLimit = strVector.size();
        for (int i = gpuNameLineVector[n]; i < upperLimit; i++)
        {
            QString str = strVector[i];
            if (str.indexOf("FB Memory Usage") != -1)
            {
                memoryLine = i;
            }
            if (str.indexOf("Temperature") != -1)
            {
                temperatureLine = i;
            }
            if (str.indexOf("Power Readings") != -1)
            {
                powerLine = i;
            }
            if (str.indexOf("Utilization") != -1)
            {
                utilizationLine = i;
            }
        }

        // Total memory
        QString str = strVector[memoryLine + 1];
        int pos = str.indexOf("Total");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            int memoryTotal = str.mid(pos, lastPos - pos).toInt();
            memoryTotalVector[n] = memoryTotal;
        }

        // Used memory
        str = strVector[memoryLine + 2];
        pos = str.indexOf("Used");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            int memoryUsed = str.mid(pos, lastPos - pos).toInt();
            memoryUsedVector[n] = memoryUsed;
        }

        // Temperature
        str = strVector[temperatureLine + 1];
        pos = str.indexOf("GPU Current Temp");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            int temperature = str.mid(pos, lastPos - pos).toInt();
            temperatureVector[n] = temperature;
        }

        // Power draw
        str = strVector[powerLine + 2];
        pos = str.indexOf("Power Draw");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            float powerDraw = str.mid(pos, lastPos - pos).toFloat();
            powerDrawVector[n] = powerDraw;
        }

        // Power limit
        str = strVector[powerLine + 3];
        pos = str.indexOf("Power Limit");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            float powerLimit = str.mid(pos, lastPos - pos).toFloat();
            powerLimitVector[n] = powerLimit;
        }

        // GPU utilization
        str = strVector[utilizationLine + 1];
        pos = str.indexOf("Gpu");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            int gpuUtil = str.mid(pos, lastPos - pos).toInt();
            gpuUtilVector[n] = gpuUtil;
        }

        // Memory utilization
        str = strVector[utilizationLine + 2];
        pos = str.indexOf("Gpu");
        if (pos != -1)
        {
            pos = str.indexOf(":") + 2;
            int lastPos = str.lastIndexOf(' ');
            int memoryUtil = str.mid(pos, lastPos - pos).toInt();
            memoryUtilVector[n] = memoryUtil;
        }
    }
}

void MainWindow::updateControl()
{
    if (gpuNum < 1)
        return;

    for (int i = 0; i < gpuNum; i++)
    {
        QString memory = QString("%1 MB / %2 MB").arg(memoryUsedVector[i]).arg(memoryTotalVector[i]);
        memoryEdit[i]->setText(memory);

        QString gpuUtil = QString("%1 %").arg(gpuUtilVector[i]);
        gpuUtilEdit[i]->setText(gpuUtil);

        QString power = QString("%1 W / %2 W").arg(powerDrawVector[i]).arg(powerLimitVector[i]);
        powerEdit[i]->setText(power);

        QString temperature = QString("%1 °C").arg(temperatureVector[i]);
        temperatureEdit[i]->setText(temperature);

        QString actionText = QString("GPU %1 :  %2 MB / %3 MB,  %4 %,  %5 °C").arg(i).
                                    arg(memoryUsedVector[i]).arg(memoryTotalVector[i]).
                                    arg(gpuUtilVector[i]).
                                    arg(temperatureVector[i]);
        gpuInfoAction[i]->setText(actionText);
    }
}

// Get Nvidia-smi output
QStringList MainWindow::getCommandOutput()
{
    QStringList strVector;
#ifdef  __linux
    FILE* pp = popen("nvidia-smi -q", "r");
#else
    FILE* pp = _popen("C:\\\"Program Files\"\\\"NVIDIA Corporation\"\\NVSMI\\nvidia-smi.exe -q", "r");
#endif
    if (!pp)
        return strVector;

    char tmp[1024];
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        if (tmp[strlen(tmp) - 1] == '\n')
        {
            // Remove newline
            tmp[strlen(tmp) - 1] = '\0';
        }
        strVector.append(tmp);
    }
#ifdef  __linux
    pclose(pp);
#else
    _pclose(pp);
#endif

    return strVector;
}

void MainWindow::createIconGroupBox()
{
    if (gpuNum < 1)
        return;

    gpuGroupBox = new QGroupBox*[gpuNum];
    nameLabel = new QLabel*[gpuNum];
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
        gpuGroupBox[i] = new QGroupBox(tr("GPU %1:").arg(i));

        nameLabel[i] = new QLabel(gpuName[0]);
        memoryLabel[i] = new QLabel(tr("Memory Usage:"));
        memoryEdit[i] = new QLineEdit;
        memoryEdit[i]->setMinimumWidth(150);
        memoryEdit[i]->setMaximumWidth(150);
        memoryEdit[i]->setFocusPolicy(Qt::NoFocus);
        gpuUtilLabel[i] = new QLabel(tr("GPU Util:"));
        gpuUtilEdit[i] = new QLineEdit;
        QSize size = gpuUtilEdit[i]->minimumSizeHint();
        gpuUtilEdit[i]->setMaximumWidth(size.width() + 10);
        gpuUtilEdit[i]->setFocusPolicy(Qt::NoFocus);
        powerLabel[i] = new QLabel(tr("Power:"));
        powerEdit[i] = new QLineEdit;
        powerEdit[i]->setMinimumWidth(150);
        powerEdit[i]->setMaximumWidth(150);
        powerEdit[i]->setFocusPolicy(Qt::NoFocus);
        temperatureLabel[i] = new QLabel(tr("Temperature:"));
        temperatureEdit[i] = new QLineEdit;
        size = temperatureEdit[i]->minimumSizeHint();
        temperatureEdit[i]->setMaximumWidth(size.width() + 10);
        temperatureEdit[i]->setFocusPolicy(Qt::NoFocus);

        QGridLayout *infoLayout = new QGridLayout;
        infoLayout->addWidget(nameLabel[i], 0, 0);
        infoLayout->addWidget(memoryLabel[i], 1, 0);
        infoLayout->addWidget(memoryEdit[i], 1, 1, 1, 2);
        infoLayout->addWidget(gpuUtilLabel[i], 1, 3, 1, 1);
        infoLayout->addWidget(gpuUtilEdit[i], 1, 4, 1, 1);
        infoLayout->addWidget(powerLabel[i], 2, 0);
        infoLayout->addWidget(powerEdit[i], 2, 1, 1, 2);
        infoLayout->addWidget(temperatureLabel[i], 2, 3, 1, 1);
        infoLayout->addWidget(temperatureEdit[i], 2, 4, 1, 1);

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
    if(checked)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
    }
    else
    {
        setWindowFlags(windowFlags() &~ Qt::WindowStaysOnTopHint);
    }
}
