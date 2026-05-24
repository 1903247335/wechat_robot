#include "pch.h"
#include"public.h"
#include "event_consumer.h"
#include"event_queue.h"
#include"message.pb.h"
#include"call_sendMessage.h"
void EventConsumer::HandleEvent(Event&event) {
	MyNetwork::MsgType type = event.type;
	//std::cout << "type:" << event.type ;
	switch (type)
	{
	case MyNetwork::SENDMESSAGE:
		DispatchToCallSendMessage(event);
		break;
	case MyNetwork::RECIVEMESSAGE:
		break;
	default:
		break;
	}
}
void EventConsumer::Run() {
	while (g_running) {
		Event event = g_EventQueue->Pop();
		HandleEvent(event);

	}
}

void EventConsumer::Stop()
{
	g_running = false;
	
}

void EventConsumer::Start() {
	
	g_thread = std::thread(&EventConsumer::Run, this);

}

