#pragma once
#include<Thread>
class EventQueue;
class Event;

class EventConsumer
{
public:
	std::thread g_thread;
	std::atomic<bool> g_running=true;
	void Start();
private:
	void HandleEvent(Event& event);

	void Run();
	void Stop();

};

