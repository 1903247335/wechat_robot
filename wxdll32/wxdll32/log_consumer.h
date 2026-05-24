#pragma once
#include"log_queue.h"
#include <thread>
class LogConsumer
{

	LogQueue* g_queue;
	std::atomic<bool>g_running;
	std::thread g_thread;
public:

	LogConsumer(LogQueue* queue);
	~LogConsumer();
	void Consume();
};

