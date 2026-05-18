// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include"client.h"
#include "public.h"
#include"login_flow.h"
#include"multiple_wechat.h"
#include"log_flow.h"

#include"log_queue.h"
#include"log_consumer.h"
#include"log_productor.h"
void OpenConsole() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
}
bool DetectWeChatDll() {
    int times = 0;

    do
    {
        Sleep(100);
        times++;
        if (times > WeChatOffsets::TRYTIMES)return false;
    } while (!GetWXModuleAddress());
    return true;
}
void StartHook() {
    if (DetectWeChatDll()) {

        LogFlow::Start();
        LoginFlow::Start();
        MultipleWechat::Start();

        
    }
    else {
        std::cout << "WeChatWin.dll Detect dll Fail" << std::endl;
    }


}

void InitLogFactory() {
	LogQueue*logQueue=new LogQueue();
	g_LogProducer = new LogProductor(logQueue);
	g_LogConsumer = new LogConsumer(logQueue);

}
DWORD WINAPI ThreadProc(LPVOID lpParameter) {

  

    OpenConsole();
 

    InitClient();
    InitQueue();
    InitLogFactory();  // 修复 LogConsumer 线程与队列生命周期后可安全启用

    StartHook();



    return 0;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

