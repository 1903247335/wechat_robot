#include "injector.h"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <tchar.h>
#include <stdio.h>
#include"tools/tools.h"
Injector::Injector() {}

bool Injector::startInject(DWORD processId, const QString &dll)
{
    if (dll.isEmpty() || processId == 0) return false;
    std::wstring wstr = Tools::URIconvertFile(dll).toStdWString();
    const TCHAR* dllPath = reinterpret_cast<const TCHAR*>(wstr.c_str());

    // 如果需要传给 WriteProcessMemory 等函数，记得计算字节大小时要乘以 sizeof(TCHAR)
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProcess == NULL)
    {
        return FALSE;
    }

    LPVOID dllPathAddress = VirtualAllocEx(hProcess, NULL, _tcslen(dllPath) * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
    if (dllPathAddress == NULL)
    {
        CloseHandle(hProcess);
        return FALSE;
    }

    SIZE_T bytesWritten;
    if (!WriteProcessMemory(hProcess, dllPathAddress, dllPath, _tcslen(dllPath) * sizeof(TCHAR), &bytesWritten))
    {
        VirtualFreeEx(hProcess, dllPathAddress, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    HMODULE kernel32Module = GetModuleHandle(_T("kernel32.dll"));
    LPTHREAD_START_ROUTINE loadLibraryFunction = (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32Module, "LoadLibraryW");
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, loadLibraryFunction, dllPathAddress, 0, NULL);
    if (hRemoteThread == NULL)
    {
        VirtualFreeEx(hProcess, dllPathAddress, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hRemoteThread, INFINITE);
    VirtualFreeEx(hProcess, dllPathAddress, 0, MEM_RELEASE);
    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);
    return TRUE;


}
