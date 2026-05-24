#pragma once
#include<string>
#include <queue>
#include <mutex>
#include <condition_variable>
namespace MyNetwork
{
	enum MsgType :int;
}
struct Event{
	MyNetwork::MsgType type;
	int id;
	std::string data;


};
class EventQueue
{
	
public:

	void Push(const Event&event);
	Event Pop(const MyNetwork::MsgType);
	Event Pop();

	void Clear();


	static void CreateNetworkEvent(MyNetwork::MsgType type, const std::string& data);
private:
	std::queue<Event> eventQueue;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	
};

