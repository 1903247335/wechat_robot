#pragma once
namespace MyNetwork{
	class Response;
}

bool HookQRCode();
extern "C" void PushQrcodeMessageToQueue(char* buf, int size);