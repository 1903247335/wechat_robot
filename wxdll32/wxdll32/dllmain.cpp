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

#include"event_queue.h"
#include"event_consumer.h"

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

static LogQueue* g_LogQueue = nullptr;

void InitLogFactory() {
	g_LogQueue = new LogQueue();
	g_LogProducer = new LogProductor(g_LogQueue);
	g_LogConsumer = new LogConsumer(g_LogQueue);
}

void ShutdownLogFactory() {
	delete g_LogConsumer;
	g_LogConsumer = nullptr;
	delete g_LogProducer;
	g_LogProducer = nullptr;
	delete g_LogQueue;
	g_LogQueue = nullptr;
}
void InitClient() {
    g_Client = new Client();
    g_Client->Connect();
}

void InitEventFactory() {
    g_EventQueue = new EventQueue();
    g_EventConsumer = new EventConsumer();

}

DWORD WINAPI ThreadProc(LPVOID lpParameter) {

  

    OpenConsole();
 
    InitEventFactory();
    InitLogFactory();

    InitClient();


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
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        ShutdownLogFactory();
        break;
    }
    return TRUE;
}

