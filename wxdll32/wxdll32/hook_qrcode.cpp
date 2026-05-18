#include"pch.h"
#include"public.h"
#include "trampoline.h"
#include"state_notice.h"
#include"event_queue.h"
#include"message.pb.h"
extern "C" {
	extern unsigned __int64 g_QrcodeNext;
	extern void QrcodeHook_Asm(void);
	void PushQrcodeMessageToQueue(char* buf, int size);
}


void PushQrcodeMessageToQueue(char*buf,int size) {

	EventQueue::CreateNetworkEvent(MyNetwork::SCANQRCODE, std::string(buf, size));

}


bool HookQRCode() {
	void* pTarget = (void*)((INT64)GetWXModuleAddress()+ WeChatOffsets::QRCODEADDRESS);
	if (!pTarget)
		return false;

	const SIZE_T trampRegion = 4096;
	BYTE* pTrampMem = (BYTE*)VirtualAlloc(NULL, trampRegion, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pTrampMem)
		return false;

	TRAMPOLINE ct{};
	ct.pTarget = pTarget;
	ct.pDetour = nullptr;
	ct.pTrampoline = pTrampMem;

	if (!CreateTrampolineFunction(&ct))
	{
		VirtualFree(pTrampMem, 0, MEM_RELEASE);
		return false;
	}

	g_QrcodeNext = (unsigned __int64)(uintptr_t)pTrampMem;




	const UINT64 detourVa = static_cast<UINT64>(reinterpret_cast<uintptr_t>(QrcodeHook_Asm));
	BYTE patch[14] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
	memcpy(patch + 6, &detourVa, sizeof(detourVa));

	DWORD oldProt = 0;
	const SIZE_T patchLen = sizeof(patch);
	if (!VirtualProtect(pTarget, patchLen, PAGE_EXECUTE_READWRITE, &oldProt))
	{
		VirtualFree(pTrampMem, 0, MEM_RELEASE);
		g_QrcodeNext = 0;
		return false;
	}
	memcpy(pTarget, patch, patchLen);
	VirtualProtect(pTarget, patchLen, oldProt, &oldProt);
	FlushInstructionCache(GetCurrentProcess(), pTarget, patchLen);
	FlushInstructionCache(GetCurrentProcess(), pTrampMem, trampRegion);

	return true;


};