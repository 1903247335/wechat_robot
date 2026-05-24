#include"pch.h"
#include"public.h"
#include"client.h"
#include"event_queue.h"
#include"message.pb.h"

#include <winsock2.h>
#include<ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

Client::Client() {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "Winsock init fail" << std::endl;
		return;
	}

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "create sock fail" << std::endl;
		WSACleanup();
		return;
	}
	

}

Client::~Client() {
	closesocket(clientSocket);
	WSACleanup();
}
void Client::Connect() {
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Connected to server fail" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	std::cout << "Connected to server!" << std::endl;

	receThread=std::thread(&Client::ReceiveMessage,this);
}



void Client::SendMyMessage(const std::string& message) {
	int result = send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "Send message fail" << std::endl;
	}
	else {
		std::cout << "Send message size: " << message.size() << std::endl;
	}
}
void Client::SendMyMessage(char*message,int size) {
	int result = send(clientSocket, message, size, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "Send message fail" << std::endl;
	}
	else {
		std::cout << "Sent message: " << message << std::endl;
	}
}

void Client::ReceiveMessage()
{
	char buffer[RECEIVE_BUFFER_SIZE] = {0};
	std::vector<char> recvBuffer;

	while (recvThread_isRunning)
	{
		int len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
		if (len > 0) {
			recvBuffer.insert(recvBuffer.end(), buffer, buffer + len);

			//if (buffer[len - 1] == '\n') break;
			std::memset(buffer, 0, sizeof(buffer));
		}
		else if (len == 0) {
			std::cout << "Connection closed by server." << std::endl;
		}
		else {
			std::cerr << "Receive message fail" << std::endl;
		}
		if (!recvBuffer.empty()) {
			std::string finalMessage(recvBuffer.begin(), recvBuffer.end());
			std::cout << "Received complete message: " << finalMessage << std::endl;
			HandleMessage(finalMessage);
		}
	}

	
}

void Client::StopReceiveThread()
{
	recvThread_isRunning = false;
}

void Client::HandleMessage(const std::string& message)
{
	MyNetwork::Request request;
	if (!request.ParseFromString(message)) {
		std::cerr << "Failed to parse message." << std::endl;
		return;
	}
	EventQueue::CreateNetworkEvent(request.type(), request.data());
}
