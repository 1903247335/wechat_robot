#pragma once

#include <string>
#include <random> 
#include "wechar_offsets.h"
#include "client.h"


class Client;
class EventQueue;
class LogProductor;
class LogConsumer;
#define MSG(msgText) MessageBoxW(NULL,msgText,L"???",MB_OK)

extern Client* g_Client;
extern EventQueue* g_EventQueue;
extern LogProductor* g_LogProducer;
extern LogConsumer* g_LogConsumer;


std::wstring GenerateRandomString(size_t length);
INT64 GetWXModuleAddress();
void InitClient();
void InitQueue();
std::string WStringToString(const wchar_t* wstr);extern "C"  void MySafeLog(wchar_t* msg);
