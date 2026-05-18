#include "pch.h"
#include "public.h"
#include "log_consumer.h"
#include"event_queue.h"
#include"state_notice.h"
#include"message.pb.h"
MyNetwork::Response CreateResponse(MyNetwork::MsgType type, const std::string& data) {
	MyNetwork::Response msg;
	msg.set_id(GetCurrentThreadId());
	msg.set_type(type);
	if (data.empty()) {
		msg.set_data(nullptr, 0);
	}
	else {
		msg.set_data(data.data(), data.size());
	}


	return msg;
}

MyNetwork::Response FromLogCreateResponse(std::wstring_view logMessage) {

	if (logMessage.find(L"getLoginQRcode success") != std::wstring::npos) {
		Event event = g_EventQueue->Pop(MyNetwork::SCANQRCODE);
		std::cout << "FromLogCreateResponse: getLoginQRcode success, data size: " << event.data.size() << std::endl;
		return CreateResponse(MyNetwork::SCANQRCODE, event.data);
	}
	else if (logMessage.find(L"Receive a LOGIN_QRCOCE_NOTIFY") != std::wstring::npos) {
		Event event = g_EventQueue->Pop(MyNetwork::WAITAUTH);
		return CreateResponse(MyNetwork::WAITAUTH, event.data);

	}
	else if (logMessage.find(L"ON_NETSCENE_AUTH_ERR_SHOW_XML") != std::wstring::npos) {

		return CreateResponse(MyNetwork::LOGINFAIL, "");
	}
	else if (logMessage.find(L"On QR Code Confirmed Start Invoke Manual Auth") != std::wstring::npos) {
		return CreateResponse(MyNetwork::AUTHSUCCESS, "");
	}
	else if (logMessage.find(L"Login Success") != std::wstring::npos) {
		return CreateResponse(MyNetwork::LOGINSUCCESS, "");
	}
	else {
		//std::wcout << L"Cannot Find Mode " << logMessage << std::endl;
		return  CreateResponse(MyNetwork::UNKNOW, "");;
	}
	;

}
LogConsumer::LogConsumer(LogQueue* queue)
	
{
	g_queue = queue;
	g_thread =std::thread(&LogConsumer::Consume, this);
}

void LogConsumer::Consume()
{
	while (true)
	{
		const std::wstring msg = g_queue->Pop();
		MyNetwork::Response response = FromLogCreateResponse(msg);
		HandleStateNotice(response);
	}
}
