#include "pch.h"
#include "public.h"
#include"event_queue.h"
#define OPENCONSOLE


Client* g_Client = nullptr;
EventQueue* g_EventQueue = nullptr;
LogProductor* g_LogProducer=nullptr;
LogConsumer* g_LogConsumer = nullptr;

INT64 GetWXModuleAddress()
{
    return (INT64)GetModuleHandleA("WeChatWin.dll");
}



std::wstring GenerateRandomString(size_t length) {
	// 1. �����ѡ�ַ���Χ
	const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	// 2. ��ʼ�������������
	std::random_device rd;  // ���ڻ�ȡ���������
	std::mt19937 generator(rd()); // �����ӳ�ʼ������

	std::uniform_int_distribution<size_t> distribution(0, characters.size() - 1);

	std::string random_string;
	for (size_t i = 0; i < length; ++i) {
		random_string += characters[distribution(generator)];
	}
	
	return std::wstring(random_string.begin(), random_string.end());;
}


void InitClient() {
	g_Client = new Client();
	g_Client->Connect();
}

void InitQueue() {
	g_EventQueue = new EventQueue();

}

std::string WStringToString(const wchar_t* wstr)
{
    if (!wstr)
        return {};

    int size = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr
    );

    std::string result(size - 1, 0);

    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr,
        -1,
        result.data(),
        size,
        nullptr,
        nullptr
    );

    return result;
}

