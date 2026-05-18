#include "starter.h"
#include "injector.h"
#include <qdebug.h>
#include"tools/tools.h"
Starter::Starter() {}

bool Starter::startWeChat(const QString& exePath,const QString&dllPath)
{
    std::wstring wPath=Tools::URIconvertFile(exePath).toStdWString();
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // 1. 以挂起模式启动进程 (CREATE_SUSPENDED)
    BOOL success = CreateProcessW(
        wPath.c_str(),
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED, // 关键：挂起启动
        NULL,
        NULL,
        &si,
        &pi
        );

    if (!success) {
        qDebug() << "<挂起线程>错误代码 (Error Code):" << GetLastError();
        return false;
    }

    DWORD pid = pi.dwProcessId;
    wchar_t eventName[64];
    swprintf(eventName, 64, L"Global\\WXInject_%u", pid);

    // 2. 创建该唯一事件
    HANDLE hEvent = CreateEventW(NULL, FALSE, FALSE, eventName);

    Injector injector;
    bool isInjectorSuccess=injector.startInject(pi.dwProcessId, dllPath);

    WaitForSingleObject(hEvent, 10000);


    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return isInjectorSuccess;
}
