#include "pch.h"
#include "log_queue.h"

void LogQueue::Push(std::wstring msg)
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		g_queue.push(std::move(msg));
	}
	g_cv.notify_one();
}

std::wstring LogQueue::Pop()
{

	std::unique_lock<std::mutex> lock(g_mutex);
	while (g_queue.empty()) {
		g_cv.wait(lock);
	}
	std::wstring wMsg = std::move(g_queue.front());
	g_queue.pop();
	return wMsg;
}
