#include"pch.h"
#include"client.h"
#include"public.h"
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
