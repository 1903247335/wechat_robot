#pragma once
#include <winsock2.h>
#include<thread>
class Client {
public:
	SOCKET clientSocket;
	std::thread receThread;
	std::atomic<bool>recvThread_isRunning = true;

	Client();
	~Client();
	void Connect();
	void SendMyMessage(const std::string& message);
	void SendMyMessage(char* message, int size);
	void ReceiveMessage();
	void StopReceiveThread();

	void HandleMessage(const std::string& message);
};