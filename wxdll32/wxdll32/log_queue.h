#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <string>

class LogQueue
{
	std::queue<std::wstring> g_queue;
	std::mutex g_mutex;
	std::condition_variable g_cv;
	std::atomic<bool> g_stop{ false };

public:
	void Push(std::wstring msg);
	void Clear();
	// 停止后且队列为空时返回空字符串
	std::wstring Pop();
	void Stop();
	bool IsStopped() const { return g_stop.load(); }
};
