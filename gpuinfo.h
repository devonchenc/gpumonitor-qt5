#ifndef GPUINFO_H
#define GPUINFO_H

#include <QThread>
#include <QTimer>
#include <QStringList>

class GPUInfo : public QThread
{
    Q_OBJECT

public:
    static GPUInfo* getInstance();

    static int getGPUNum()                          { return gpuNum; }
    static const QStringList& getGPUName()          { return gpuName; }
    static QVector<int>& getMemoryTotalVector()     { return memoryTotalVector; }
    static QVector<int>& getMemoryUsedlVector()     { return memoryUsedVector; }
    static QVector<int>& getTemperatureVector()     { return temperatureVector; }
    static QVector<float>& getPowerDrawVector()     { return powerDrawVector; }
    static QVector<QString>& getPowerLimitVector()  { return powerLimitVector; }
    static QVector<int>& getGPUUtilVector()         { return gpuUtilVector; }
    static QVector<int>& getMemoryUtilVector()      { return memoryUtilVector; }

signals:
    void infoUpdated();

protected:
    void run() override;

private:
    int getInfo();

    void updateInfo();

    // Get Nvidia-smi output
    static QStringList getCommandOutput();

    GPUInfo(QObject *parent = nullptr) : QThread(parent){}
    ~GPUInfo(){if(timer){timer->deleteLater();}}

private:
    // instance pointer
    static GPUInfo* instance;

    static int gpuNum;
    static QStringList gpuName;
    static QVector<int> gpuNameLineVector;
    static QVector<int> memoryTotalVector;
    static QVector<int> memoryUsedVector;
    static QVector<int> temperatureVector;
    static QVector<float> powerDrawVector;
    static QVector<QString> powerLimitVector;
    static QVector<int> gpuUtilVector;
    static QVector<int> memoryUtilVector;

    static QTimer* timer;

private:
    class GC // Garbage collection
    {
    public:
        ~GC()
        {
            if (instance != nullptr)
            {
                delete instance;
                instance = nullptr;
            }
        }
    };
    static GC gc;
};

#endif // GPUINFO_H
