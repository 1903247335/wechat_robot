#include"pch.h"
#include"public.h"
#include"call_sendMessage.h"
#include"event_queue.h"
#include"message.pb.h"
#include"wechat_offsets.h"

MyNetwork::Request EventToRequest(Event& event) {

	MyNetwork::Request request;
	request.set_data(event.data.data(), event.data.size());
	request.set_id(event.id);
	request.set_type(event.type);
	return request;
};

void CallSendMessage(MyNetwork::SendMessageRequest& req) {
		pCallSendMessage callSendMessage =(pCallSendMessage)(GetWXModuleAddress() + WeChatOffsets::SEND_MESSAGE_ADDRESS);
		
		//std::wstring wWxid = StringToWstring(req.wxid());

		std::wstring wContent = StringToWstring(req.content());
		std::wstring wWxid = L"filehelper";
		//std::wstring wContent = req.content;

		MMWString receiver = {
			const_cast<wchar_t*>(wWxid.c_str()),
			(int)wWxid.size(),
			nullptr
		};
		MMWString content = {
			const_cast<wchar_t*>(wContent.c_str()),
			(int)wContent.size(),
			nullptr
		};

		void*ploc=malloc(0x500);
		memset(ploc,0, 0x500);
		callSendMessage(ploc, &receiver,&content, 0, (int)req.msg_type(), 0, 0, 0);
}

MyNetwork::SendMessageRequest  RequestToSendMessageRequest( MyNetwork::Request& msg) {
	MyNetwork::SendMessageRequest req;
	req.ParseFromString(msg.data());

	return req;
	
}
	void DispatchToCallSendMessage(Event&event)
{
	MyNetwork::Request request=EventToRequest(event);
	MyNetwork::SendMessageRequest msg=RequestToSendMessageRequest(request);
	CallSendMessage(msg);
}
