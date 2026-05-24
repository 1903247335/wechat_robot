#include "pch.h"
#include "log_productor.h"

LogProductor::LogProductor(LogQueue* queue)
{

	g_queue = queue;
}

void LogProductor::Produce(wchar_t* msg)
{
	{
		//std::lock_guard<std::mutex> lock(g_mutex);

		g_queue->Push(std::wstring(msg));
		//g_queue->Pop();
	}

}
