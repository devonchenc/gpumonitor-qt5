#include "gpuinfo.h"

#include <QVector>
#include <QStringList>

#ifdef _WIN32
#include <Windows.h>
#endif

GPUInfo* GPUInfo::instance = Q_NULLPTR;
int GPUInfo::gpuNum = 0;
QStringList GPUInfo::gpuName;
QVector<int> GPUInfo::gpuNameLineVector;
QVector<int> GPUInfo::memoryTotalVector;
QVector<int> GPUInfo::memoryUsedVector;
QVector<int> GPUInfo::temperatureVector;
QVector<float> GPUInfo::powerDrawVector;
QVector<QString> GPUInfo::powerLimitVector;
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
    memoryLine = temperatureLine = powerLine = utilizationLine = 0;
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
            powerLimitVector[n] = str.mid(pos);
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
    SECURITY_ATTRIBUTES sa = { 0 };
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    // Create pipe for standard output redirection.
    HANDLE hPipeOutputRead = NULL;
    HANDLE hPipeOutputWrite = NULL;
    CreatePipe(&hPipeOutputRead, &hPipeOutputWrite, &sa, 0);

    // Make child process use hPipeOutputWrite as standard out, and make sure it does not show on screen.
    STARTUPINFO si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = hPipeOutputWrite;
    si.hStdError = hPipeOutputWrite;
    PROCESS_INFORMATION pi = { 0 };

    // Create nvidia-smi.exe as child process
    TCHAR szCmd[256] = L"C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvidia-smi.exe -q";
    CreateProcess(NULL, szCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
    CloseHandle(hPipeOutputWrite);

    char szBuffer[512];
    QString totalOutput;
    while (TRUE)
    {
        DWORD dwNumberOfBytesRead = 0;
        BOOL bTest = ReadFile(hPipeOutputRead, &szBuffer, 512 - 1, &dwNumberOfBytesRead, NULL);
        if (!bTest)
        {
            break;
        }

        szBuffer[dwNumberOfBytesRead] = 0;  // null terminate

        // Concatenate output string
        totalOutput += szBuffer;
    }

    // Wait for shell to finish.
    WaitForSingleObject(pi.hProcess, 2000);

    // Close all remaining handles
    CloseHandle(pi.hProcess);
    CloseHandle(hPipeOutputRead);

    // Split the output string by line break
    strVector = totalOutput.split("\r\n");
#else
    FILE* pp = popen("nvidia-smi -q", "r");
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
    pclose(pp);
#endif

    return strVector;
}
