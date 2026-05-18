#pragma once
#include <windows.h>

typedef HANDLE(WINAPI* PfnCreateMutexW)(LPSECURITY_ATTRIBUTES, BOOL, LPCWSTR);
bool HookMutex();