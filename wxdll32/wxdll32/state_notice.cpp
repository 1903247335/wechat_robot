#include"pch.h"
#include "state_notice.h"
#include"public.h"
#include"message.pb.h"

bool IsClientInitialized() {
	if (!g_Client) {
		std::cerr << "Client is not initialized." << std::endl;
		return false;
	}
	return true;
}
bool IsNotificationState(const MyNetwork::MsgType& type) {
	return (type >= MyNetwork::NOTIFICATION_BEGIN && type <= MyNetwork::NOTIFICATION_END);

}

bool ProcessStateNotice(const MyNetwork::Response& msg) {
	
	std::string serializedData;
	if (!msg.SerializeToString(&serializedData)) {
		return false;
	}

	if (g_Client) {
		g_Client->SendMyMessage(serializedData);
		return true;
	}
	return false;
	
}
bool HandleStateNotice(const MyNetwork::Response& msg)
	
{
	if (!IsNotificationState(msg.type())) {
		return false;
	}
	if (!IsClientInitialized()) {
		return false;
	}
	

	return ProcessStateNotice( msg);
}
