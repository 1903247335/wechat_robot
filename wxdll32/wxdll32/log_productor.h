#pragma once
#include"log_queue.h"
class LogProductor
{
	LogQueue* g_queue;
public:
	LogProductor(LogQueue*queue);

	void Produce(wchar_t* msg);
};

