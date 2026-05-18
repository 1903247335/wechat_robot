#pragma once
bool  HookLog();
void printDebugger();
void UnHookLog();

typedef char* (*LogMessage)(
    int a1,
    int a2,
    int a3,
    int a4,
    int a5,
    const char* a6,
    const char* a7,
    LONGLONG* a8,
    LONGLONG* a9,
    LONGLONG* a10,
    LONGLONG* a11,
    LONGLONG* a12,
    LONGLONG* a13,
    ...
    );