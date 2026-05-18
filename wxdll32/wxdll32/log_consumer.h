#pragma once
#include"log_queue.h"
#include <thread>
class LogConsumer
{

	LogQueue* g_queue;
	std::thread g_thread;
public:

	LogConsumer(LogQueue* queue);
		void Consume();
};

