#include "gpuinfo.h"

#include <QVector>
#include <QStringList>

GPUInfo* GPUInfo::instance = Q_NULLPTR;
int GPUInfo::gpuNum = 0;
QStringList GPUInfo::gpuName;
QVector<int> GPUInfo::gpuNameLineVector;
QVector<int> GPUInfo::memoryTotalVector;
QVector<int> GPUInfo::memoryUsedVector;
QVector<int> GPUInfo::temperatureVector;
QVector<float> GPUInfo::powerDrawVector;
QVector<float> GPUInfo::powerLimitVector;
QVector<int> GPUInfo::gpuUtilVector;
QVector<int> GPUInfo::memoryUtilVector;
GPUInfo::GC GPUInfo::gc;

GPUInfo* GPUInfo::getInstance()
{
    if (instance == Q_NULLPTR)
    {
        instance = new GPUInfo;
        getInfo();
        updateInfo();
    }
    return instance;
}

void GPUInfo::cleanUp()
{
    printf("cleanUp");
    if (instance != Q_NULLPTR)
    {
        delete instance;
    }
}

int GPUInfo::getInfo()
{
    QStringList strVector = getCommandOutput();
    if (strVector.size() == 0)
        return 0;

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

void GPUInfo::updateInfo()
{
    if (gpuNum == 0)
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

// Get Nvidia-smi output
QStringList GPUInfo::getCommandOutput()
{
    QStringList strVector;
#ifdef _WIN32
    FILE* pp = _popen("C:\\\"Program Files\"\\\"NVIDIA Corporation\"\\NVSMI\\nvidia-smi.exe -q", "r");
#else
    FILE* pp = popen("nvidia-smi -q", "r");
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
#ifdef _WIN32
    _pclose(pp);
#else
    pclose(pp);
#endif

    return strVector;
}
