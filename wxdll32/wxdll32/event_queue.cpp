#include "pch.h"
#include "event_queue.h"
#include "public.h"
#include"message.pb.h"
void EventQueue::Push(const Event& event)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		eventQueue.push(event);
		//std::cout << "Event pushed: type=" << event.type << ", data size=" << event.data.size() << std::endl;
	}
	m_cv.notify_one();
}
Event EventQueue::Pop(const MyNetwork::MsgType type) {

		std::unique_lock<std::mutex> lock(m_mutex);
		while (true)
		{
			m_cv.wait(lock, [this] {
				return !eventQueue.empty();
				});

			size_t count = eventQueue.size();
			//std::cout << "Queue size =" << count <<"Need find type= " << type << std::endl;
			for (size_t i = 0; i < count; ++i)
			{
				Event ev = eventQueue.front();
				eventQueue.pop();

				if (ev.type == type)
				{
		
					return ev;
				}

				eventQueue.push(ev);
			}
			//std::cout << "�Ҳ���Ŀ�� type=" << type << std::endl;

			if (type == MyNetwork::WAITAUTH|| type == MyNetwork::UNKNOW) {
				throw "Can not find Targt Type";
			}

			m_cv.wait(lock);
		}

		

	
}
Event EventQueue::Pop() {
	std::unique_lock<std::mutex> lock(m_mutex);
	while (eventQueue.empty()) {
		m_cv.wait(lock);
	}
	Event ev = eventQueue.front();
	eventQueue.pop();
	return ev;
}
void EventQueue::Clear() {
	std::lock_guard<std::mutex> lock(m_mutex);
	while (!eventQueue.empty()) {
		eventQueue.pop();
	}
}

void EventQueue::CreateNetworkEvent(MyNetwork::MsgType type, const std::string& data)
{
		Event event;
		event.type = type;
		event.data = data;
		g_EventQueue->Push(event);
	
}
