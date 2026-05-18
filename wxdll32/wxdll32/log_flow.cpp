#include "pch.h"
#include "log_flow.h"
#include"hook_log.h"
void LogFlow::Start()
{
    if (HookLog()) {
        std::cout << "WeChatWin.dll HOOK Log Success" << std::endl;
    }
    else {
        std::cout << "WeChatWin.dll HOOK Log Error" << std::endl;

    }

}
