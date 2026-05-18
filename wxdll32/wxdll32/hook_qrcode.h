#pragma once
#include"message.pb.h"
bool HookQRCode();
MyNetwork::Response CreateQrcodeResponse(char* buf, int size);
extern "C" void PushQrcodeMessageToQueue(char* buf, int size);