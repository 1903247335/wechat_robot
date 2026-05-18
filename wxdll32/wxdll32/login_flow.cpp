#include "pch.h"
#include "login_flow.h"
#include"hook_qrcode.h"
#include"hook_head.h"
void LoginFlow::Start()
{
    if (HookQRCode()) {
        std::cout << "WeChatWin.dll HOOK qrcode Success" << std::endl;
    }
    else {
        std::cout << "WeChatWin.dll HOOK qrcode Error" << std::endl;
    }
    if (HookHead())
    {
        std::cout << "WeChatWin.dll HOOK head Success" << std::endl;
    }
    else {
        std::cout << "WeChatWin.dll HOOK head Error" << std::endl;
    }




}
