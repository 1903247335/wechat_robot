#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <string>
class LogQueue
{
	std::queue<std::wstring> g_queue;
	std::mutex g_mutex;
	std::condition_variable g_cv;
public:
	void Push(std::wstring msg);
	std::wstring Pop();
};

