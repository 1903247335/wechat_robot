#include "pch.h"

#include "public.h"

#include "hook_log.h"

#include "trampoline.h"

#include "message.pb.h"

#include"state_notice.h"

#include "event_queue.h"

#include"log_productor.h"

#include<wchar.h>

#include <mutex>



typedef void* (__fastcall* pWeChatLog)(int level, const char* file, int line, const char* func, ...);



extern "C" {

	extern unsigned __int64 g_LogBack;

	extern void WeChatLogHook_Asm(void);

	void HandleLog(wchar_t*);

	void HandleLogHook(

		const wchar_t* fmt,



		int line,

		const wchar_t* func);

}







extern "C" void HandleLogHook(

	const wchar_t*fmt,



	int line,

	const wchar_t* func
)

{








	wchar_t wbuf[4096] = {};

	_snwprintf_s(

		wbuf,

		_TRUNCATE,

		L"[%d]  %hs %S",
		
		line,
		func,
		fmt
		);



	HandleLog(wbuf);



}



std::wstring_view WcharToWstring(const wchar_t* wstr) {

	return std::wstring_view(wstr);

}



void HandleLog(wchar_t* msg) {


	
	//MySafeLog(msg);



	g_LogProducer->Produce(msg);

	

}



void MySafeLog(wchar_t* msg) {

	wprintf(L"LOG: %s\n", msg);

}



static void DebugPrintA(const char* msg)

{

	printf(msg);

}



bool HookLog() {

	const SIZE_T trampRegion = 4096;

	BYTE* pMemory = (BYTE*)VirtualAlloc(NULL, trampRegion, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!pMemory)

		return false;



	TRAMPOLINE ct{};



	LPVOID pTarget = (LPVOID)(GetWXModuleAddress() + WeChatOffsets::LOG_ADDRESS);

	MEMORY_BASIC_INFORMATION mbi{};

	if (VirtualQuery(pTarget, &mbi, sizeof(mbi)) == 0

		|| mbi.State != MEM_COMMIT

		|| !(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))

	{

		VirtualFree(pMemory, 0, MEM_RELEASE);

		DebugPrintA("HookLog: target is not executable.\n");

		return false;

	}



	ct.pTarget = pTarget;

	ct.pDetour = nullptr;

	ct.pTrampoline = pMemory;

	std::wcout << L"HookLog: pTarget = " << std::hex << pTarget << std::endl;

	if (!CreateTrampolineFunction(&ct))

	{

		DebugPrintA("HookLog: CreateTrampolineFunction failed.\n");

		VirtualFree(pMemory, 0, MEM_RELEASE);

		return false;

	}



	g_LogBack = (unsigned __int64)(uintptr_t)pMemory;

	BYTE patch[14] = { 0xFF,0x25,00,00,00,00 };

	const UINT64 detourVa = static_cast<UINT64>(reinterpret_cast<uintptr_t>(WeChatLogHook_Asm));

	memcpy(patch + 6, &detourVa, sizeof(detourVa));



	DWORD oldProt = 0;

	const SIZE_T patchLen = sizeof(patch);

	if (!VirtualProtect(pTarget, patchLen, PAGE_EXECUTE_READWRITE, &oldProt))

	{

		DebugPrintA("HookLog: VirtualProtect failed.\n");

		VirtualFree(pMemory, 0, MEM_RELEASE);

		g_LogBack = 0;

		return false;

	}

	memcpy(pTarget, patch, patchLen);

	VirtualProtect(pTarget, patchLen, oldProt, &oldProt);

	FlushInstructionCache(GetCurrentProcess(), pTarget, patchLen);

	FlushInstructionCache(GetCurrentProcess(), pMemory, trampRegion);



	return true;

}


