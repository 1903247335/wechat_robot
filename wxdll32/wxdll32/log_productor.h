#pragma once
#include"log_queue.h"
#include<mutex>
class LogProductor
{
	LogQueue* g_queue;
	std::mutex g_mutex;
	int g_inc;
public:
	LogProductor(LogQueue*queue);

	void Produce(wchar_t* msg);
};

