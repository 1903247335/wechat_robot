#include "pch.h"
#include "log_queue.h"

void LogQueue::Stop()
{
	{
		std::lock_guard<std::mutex> lock(g_mutex);
		g_stop.store(true);
	}
	g_cv.notify_all();
}

void LogQueue::Push(std::wstring msg)
{

	if (msg.empty())return;
	{
		
		std::lock_guard<std::mutex> lock(g_mutex);

		g_queue.push(std::move(msg));
	}


	g_cv.notify_one();
}
void LogQueue::Clear() {
	//std::unique_lock<std::mutex> lock(g_mutex);
	while (!g_queue.empty()) {
		g_queue.pop();
	}


};
std::wstring LogQueue::Pop()
{

	std::unique_lock<std::mutex> lock(g_mutex);
	g_cv.wait(lock, [this] {
		return  !g_queue.empty();
	});


	std::wstring wMsg = std::move(g_queue.front());
	g_queue.pop();
	return wMsg;
}
