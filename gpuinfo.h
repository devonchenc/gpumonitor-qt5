#ifndef GPUINFO_H
#define GPUINFO_H

#include <QStringList>

class GPUInfo
{
public:
    static GPUInfo* getInstance();
    static void cleanUp();

    static void updateInfo();

    static int getGPUNum()                          { return gpuNum; }

    static const QStringList& getGPUName()          { return gpuName; }
    static QVector<int>& getMemoryTotalVector()     { return memoryTotalVector; }
    static QVector<int>& getMemoryUsedlVector()     { return memoryUsedVector; }
    static QVector<int>& getTemperatureVector()     { return temperatureVector; }
    static QVector<float>& getPowerDrawVector()     { return powerDrawVector; }
    static QVector<float>& getPowerLimitVector()    { return powerLimitVector; }
    static QVector<int>& getGPUUtilVector()         { return gpuUtilVector; }
    static QVector<int>& getMemoryUtilVector()      { return memoryUtilVector; }

private:
    static int getInfo();

    // Get Nvidia-smi output
    static QStringList getCommandOutput();

    GPUInfo(){}

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
    static QVector<float> powerLimitVector;
    static QVector<int> gpuUtilVector;
    static QVector<int> memoryUtilVector;

private:
    class GC // Garbage collection
    {
    public:
        ~GC()
        {
            if (instance != Q_NULLPTR)
            {
                delete instance;
                instance = NULL;
            }
        }
    };
    static GC gc;
};

#endif // GPUINFO_H
