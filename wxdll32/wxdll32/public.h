#pragma once

#include <string>
#include <random> 
#include "wechat_offsets.h"
#include "client.h"
#define OPENCONSOLE
#define RECEIVE_BUFFER_SIZE 10240

class Client;
class EventQueue;
class LogProductor;
class LogConsumer;
class EventConsumer;


extern Client* g_Client;
extern EventQueue* g_EventQueue;
extern LogProductor* g_LogProducer;
extern LogConsumer* g_LogConsumer;
extern EventConsumer* g_EventConsumer;

std::wstring GenerateRandomString(size_t length);
INT64 GetWXModuleAddress();

std::string WStringToString(const wchar_t* wstr);
std::wstring StringToWstring(const std::string& str);

extern "C"  void MySafeLog(wchar_t* msg);
