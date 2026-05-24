#pragma once
struct MMWString {
    wchar_t* data;
    int      len;
    void* cap;  // Мо nullptr
};
typedef void(*pCallSendMessage)(void* chatMsgOut,
    MMWString* receiver,
    MMWString* content,
    __int64    extra,
    int        msgType,
    int        xmlMerge,
    int        unused,
    void* msgWrap);

namespace MyNetwork
{
	class Request;
	class SendMessageRequest;
}
class Event;

void CallSendMessage(MyNetwork::SendMessageRequest& req);
MyNetwork::SendMessageRequest RequestToSendMessageRequest( MyNetwork::Request& msg);
void DispatchToCallSendMessage(Event&);