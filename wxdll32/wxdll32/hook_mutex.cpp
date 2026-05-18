#include"pch.h"
#include"hook_mutex.h"
#include "trampoline.h"
#include"public.h"

extern "C" {
	extern unsigned __int64 g_CreateMutexWTrampoline;
	void ModifyMutexString(wchar_t* pAddress);
	void CreateMutexW_Detour_Asm(void);

}
void SignalInjectReady()
{
	DWORD pid = GetCurrentProcessId();
	wchar_t eventName[64];
	swprintf_s(eventName, _countof(eventName), L"Global\\WXInject_%u", pid);

	HANDLE hEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, eventName);
	if (hEvent) {
		SetEvent(hEvent);
		CloseHandle(hEvent);
	}
}
bool HookMutex()
{

	// 只 hook KernelBase 的真实实现，避免覆盖 kernel32 的 6 字节转发桩。
	void* pTarget = GetProcAddress(GetModuleHandleA("kernelbase.dll"), "CreateMutexW");
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

	g_CreateMutexWTrampoline = (unsigned __int64)(uintptr_t)pTrampMem;




	const UINT64 detourVa = static_cast<UINT64>(reinterpret_cast<uintptr_t>(CreateMutexW_Detour_Asm));
	BYTE patch[14] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
	memcpy(patch + 6, &detourVa, sizeof(detourVa));

	DWORD oldProt = 0;
	const SIZE_T patchLen = sizeof(patch);
	if (!VirtualProtect(pTarget, patchLen, PAGE_EXECUTE_READWRITE, &oldProt))
	{
		VirtualFree(pTrampMem, 0, MEM_RELEASE);
		g_CreateMutexWTrampoline = 0;
		return false;
	}
	memcpy(pTarget, patch, patchLen);
	VirtualProtect(pTarget, patchLen, oldProt, &oldProt);
	FlushInstructionCache(GetCurrentProcess(), pTarget, patchLen);
	FlushInstructionCache(GetCurrentProcess(), pTrampMem, trampRegion);



	SignalInjectReady();
	return true;
}




extern "C" void ModifyMutexString(wchar_t* pAddress) {
	if (pAddress == NULL) return;

	// 防止 hook 回调内再次触发 CreateMutexW 导致重入崩溃
	thread_local int s_inHook = 0;
	if (s_inHook != 0) return;
	++s_inHook;

	if (wcscmp(pAddress, L"_WeChat_App_Instance_Identity_Mutex_Name") != 0) {
		--s_inHook;
		return;
	}
	std::wstring string = GenerateRandomString(4);
	std::wstring suffix = GenerateRandomString(4);
	std::wstring replacement = L"WeChat_App_Instance_Identity_Mutex_" + suffix;
	const wchar_t* kReplacement = replacement.c_str();

	const size_t oldLen = wcslen(pAddress);
	const size_t repLen = wcslen(kReplacement);
	if (repLen > oldLen) {
		--s_inHook;
		return;
	}

	const SIZE_T regionBytes = (oldLen + 1) * sizeof(wchar_t);
	DWORD oldProtect = 0;
	if (VirtualProtect(pAddress, regionBytes, PAGE_READWRITE, &oldProtect)) {
		memcpy(pAddress, kReplacement, (repLen + 1) * sizeof(wchar_t));
		VirtualProtect(pAddress, regionBytes, oldProtect, &oldProtect);
	}
	--s_inHook;
}