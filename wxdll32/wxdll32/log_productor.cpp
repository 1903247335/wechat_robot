#include "pch.h"
#include "log_productor.h"

LogProductor::LogProductor(LogQueue* queue)
{

	g_queue = queue;
}

void LogProductor::Produce(wchar_t* msg)
{
	if (!msg || !g_queue) {
		return;
	}
	g_queue->Push(std::wstring(msg));
}
