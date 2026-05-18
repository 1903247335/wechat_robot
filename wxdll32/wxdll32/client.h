#pragma once
#include <winsock2.h>
class Client {
public:
	SOCKET clientSocket;
	Client();
	~Client();
	void Connect();
	void SendMyMessage(const std::string& message);
	void SendMyMessage(char* message, int size);

};