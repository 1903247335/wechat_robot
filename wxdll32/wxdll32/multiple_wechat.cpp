#include "pch.h"
#include "multiple_wechat.h"
#include"hook_mutex.h"
void MultipleWechat::Start() {
	if (HookMutex()) {
		std::cout << "WeChatWin.dll HOOK Mutex Success" << std::endl;
	}
	else {
		std::cout << "WeChatWin.dll HOOK Mutex Error" << std::endl;
	}

};