#include"pch.h"

#include "hook_head.h"
#include "public.h"
#include "trampoline.h"
#include "message.pb.h"
#include"event_queue.h"
extern "C" {
	extern unsigned __int64 g_HeadNext;
	extern void HeadHook_Asm(void);
	void PushHeadMessageToQueue(wchar_t* url);
}
void PushHeadMessageToQueue(wchar_t* url)
{	
	std::string string = WStringToString(url);

	EventQueue::CreateNetworkEvent(MyNetwork::WAITAUTH, string);
}
bool HookHead()
{
	const SIZE_T trampRegion = 4096;
	BYTE* pMemory = (BYTE*)VirtualAlloc(NULL, trampRegion, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pMemory)
		return false;

	TRAMPOLINE ct{};

	LPVOID pTarget = (LPVOID)(GetWXModuleAddress() + WeChatOffsets::HEADADDRESS);
	MEMORY_BASIC_INFORMATION mbi{};
	if (VirtualQuery(pTarget, &mbi, sizeof(mbi)) == 0
		|| mbi.State != MEM_COMMIT
		|| !(mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)))
	{
		VirtualFree(pMemory, 0, MEM_RELEASE);
		return false;
	}

	ct.pTarget = pTarget;
	ct.pDetour = nullptr;
	ct.pTrampoline = pMemory;
	std::wcout << L"HookLog: pTarget = " << std::hex << pTarget << std::endl;
	if (!CreateTrampolineFunction(&ct))
	{
		VirtualFree(pMemory, 0, MEM_RELEASE);
		return false;
	}

	g_HeadNext = (unsigned __int64)(uintptr_t)pMemory;
	BYTE patch[14] = { 0xFF,0x25,00,00,00,00 };
	const UINT64 detourVa = static_cast<UINT64>(reinterpret_cast<uintptr_t>(HeadHook_Asm));
	memcpy(patch + 6, &detourVa, sizeof(detourVa));

	DWORD oldProt = 0;
	const SIZE_T patchLen = sizeof(patch);
	if (!VirtualProtect(pTarget, patchLen, PAGE_EXECUTE_READWRITE, &oldProt))
	{

		VirtualFree(pMemory, 0, MEM_RELEASE);
		g_HeadNext = 0;
		return false;
	}
	memcpy(pTarget, patch, patchLen);
	VirtualProtect(pTarget, patchLen, oldProt, &oldProt);
	FlushInstructionCache(GetCurrentProcess(), pTarget, patchLen);
	FlushInstructionCache(GetCurrentProcess(), pMemory, trampRegion);

	return true;

}


