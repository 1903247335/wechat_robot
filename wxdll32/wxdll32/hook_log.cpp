#include "pch.h"
#include "public.h"
#include "hook_log.h"
#include "trampoline.h"
#include "message.pb.h"
#include"state_notice.h"
#include "event_queue.h"
#include"log_productor.h"

typedef void* (__fastcall* pWeChatLog)(int level, const char* file, int line, const char* func, ...);

extern "C" {
	extern unsigned __int64 g_LogBack;
	extern void WeChatLogHook_Asm(void);
	void HandleLog(wchar_t*);
	void HandleLogHook(
		unsigned __int64 hookRsp,
		int level,
		const char* file,
		int line,
		const char* func);
}

namespace LogHook {

thread_local int g_InLogHook = 0;

bool IsReadableRange(const void* ptr, size_t len)
{
	if (!ptr || len == 0)
		return false;

	const auto start = reinterpret_cast<uintptr_t>(ptr);
	const auto end = start + len;
	if (end < start)
		return false;

	uintptr_t cur = start;
	while (cur < end) {
		MEMORY_BASIC_INFORMATION mbi{};
		if (VirtualQuery(reinterpret_cast<const void*>(cur), &mbi, sizeof(mbi)) == 0)
			return false;
		if (mbi.State != MEM_COMMIT)
			return false;
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			return false;

		const uintptr_t regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
		const uintptr_t regionEnd = regionStart + mbi.RegionSize;
		if (cur < regionStart || cur >= regionEnd)
			return false;

		const uintptr_t step = (std::min)(regionEnd, end) - cur;
		if (step == 0)
			return false;
		cur += step;
	}
	return true;
}

bool IsReadableCString(const char* s, size_t maxLen = 4096)
{
	if (!s)
		return false;
	for (size_t i = 0; i < maxLen; ++i) {
		if (!IsReadableRange(s + i, 1))
			return false;
		if (s[i] == '\0')
			return true;
	}
	return false;
}

bool SafeReadCString(const char* s, char* out, size_t outSize)
{
	if (!out || outSize == 0)
		return false;
	out[0] = '\0';
	if (!IsReadableCString(s))
		return false;

	size_t i = 0;
	for (; i + 1 < outSize; ++i) {
		out[i] = s[i];
		if (s[i] == '\0')
			return true;
	}
	out[i] = '\0';
	return false;
}

bool SafeReadStackCString(unsigned __int64 hookRsp, unsigned offset, char* out, size_t outSize)
{
	if (!IsReadableRange(reinterpret_cast<const void*>(hookRsp + offset), sizeof(const char*)))
		return false;
	const char* p = *reinterpret_cast<const char* const*>(hookRsp + offset);
	return SafeReadCString(p, out, outSize);
}

static int SafeVsnprintf(char* out, size_t outSize, const char* fmt, va_list ap)
{
	__try {
		return vsnprintf(out, outSize, fmt, ap);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return -1;
	}
}

bool TryFormatWithVaList(char* out, size_t outSize, const char* fmt, unsigned __int64 hookRsp)
{
	if (!fmt || !IsReadableCString(fmt))
		return false;

	va_list ap = reinterpret_cast<va_list>(static_cast<uintptr_t>(hookRsp + 0x38));
	return SafeVsnprintf(out, outSize, fmt, ap) >= 0;
}

bool TryFormatWithPointerSlots(char* out, size_t outSize, const char* fmt, unsigned __int64 hookRsp)
{
	if (!fmt || !IsReadableCString(fmt))
		return false;

	size_t outPos = 0;
	int argIdx = 0;

	auto appendChar = [&](char c) -> bool {
		if (outPos + 1 >= outSize)
			return false;
		out[outPos++] = c;
		return true;
	};

	auto appendLiteral = [&](const char* s) -> bool {
		if (!s)
			s = "(null)";
		while (*s) {
			if (!appendChar(*s++))
				return false;
		}
		return true;
	};

	auto appendFmt = [&](const char* f, ...) -> bool {
		if (outPos >= outSize)
			return false;
		va_list ap;
		va_start(ap, f);
		const int n = vsnprintf(out + outPos, outSize - outPos, f, ap);
		va_end(ap);
		if (n < 0)
			return false;
		outPos += static_cast<size_t>(n);
		if (outPos >= outSize)
			outPos = outSize - 1;
		return true;
	};

	for (const char* p = fmt; *p; ++p) {
		if (*p != '%') {
			if (!appendChar(*p))
				return false;
			continue;
		}
		if (p[1] == '%') {
			if (!appendChar('%'))
				return false;
			++p;
			continue;
		}

		const unsigned slotOff = 0x38u + static_cast<unsigned>(argIdx) * 8u;
		++argIdx;
		if (!IsReadableRange(reinterpret_cast<const void*>(hookRsp + slotOff), sizeof(void*)))
			return false;

		const void* slot = *reinterpret_cast<void* const*>(hookRsp + slotOff);
		if (!slot) {
			if (!appendLiteral("(null)"))
				return false;
			continue;
		}

		const char* spec = p + 1;
		while (*spec && (*spec == '-' || *spec == '+' || *spec == ' ' || *spec == '#' || *spec == '0' ||
			(*spec >= '0' && *spec <= '9') || *spec == '.' || *spec == 'h' || *spec == 'l' || *spec == 'L' ||
			*spec == 'z' || *spec == 't' || *spec == 'I'))
		{
			++spec;
		}
		if (*spec == '\0')
			break;

		const char conv = *spec;
		p = spec;

		if (!IsReadableRange(slot, sizeof(void*)))
			return false;

		switch (conv) {
		case 's': {
			char tmp[512] = {};
			const char* s = *reinterpret_cast<const char* const*>(slot);
			if (!SafeReadCString(s, tmp, sizeof(tmp)))
				return false;
			if (!appendLiteral(tmp))
				return false;
			break;
		}
		case 'd':
		case 'i':
			if (!appendFmt("%d", *reinterpret_cast<const int*>(slot)))
				return false;
			break;
		case 'u':
			if (!appendFmt("%u", *reinterpret_cast<const unsigned int*>(slot)))
				return false;
			break;
		case 'x':
		case 'X':
			if (!appendFmt("%x", *reinterpret_cast<const unsigned int*>(slot)))
				return false;
			break;
		case 'p':
			if (!appendFmt("%p", *reinterpret_cast<const void* const*>(slot)))
				return false;
			break;
		default:
			if (!appendFmt("<unsup:%c>", conv))
				return false;
			break;
		}
	}

	out[outPos] = '\0';
	return true;
}

} // namespace LogHook

extern "C" void HandleLogHook(
	unsigned __int64 hookRsp,
	int level,
	const char* file,
	int line,
	const char* func)
{
	if (LogHook::g_InLogHook > 0)
		return;

	++LogHook::g_InLogHook;

	char tagBuf[256] = {};
	char fmtBuf[512] = {};
	char body[2048] = "(format error)";

	if (!LogHook::SafeReadStackCString(hookRsp, 0x28, tagBuf, sizeof(tagBuf)))
		strncpy_s(tagBuf, "?", _TRUNCATE);
	if (!LogHook::SafeReadStackCString(hookRsp, 0x30, fmtBuf, sizeof(fmtBuf)))
		strncpy_s(fmtBuf, "?", _TRUNCATE);

	if (fmtBuf[0] != '\0' && fmtBuf[0] != '?') {
		if (!LogHook::TryFormatWithVaList(body, sizeof(body), fmtBuf, hookRsp))
			LogHook::TryFormatWithPointerSlots(body, sizeof(body), fmtBuf, hookRsp);
	}

	wchar_t wbuf[4096] = {};
	_snwprintf_s(
		wbuf,
		_TRUNCATE,
		L"[L%d] %hs:%d %hs | %hs | %hs",
		level,
		(file && LogHook::IsReadableCString(file)) ? file : "?",
		line,
		(func && LogHook::IsReadableCString(func)) ? func : "?",
		tagBuf,
		body);

	HandleLog(wbuf);
	--LogHook::g_InLogHook;
}

std::wstring_view WcharToWstring(const wchar_t* wstr) {
	return std::wstring_view(wstr);
}

void HandleLog(wchar_t* msg) {
	MySafeLog(msg);
	if (LogHook::g_InLogHook == 0 && g_LogProducer) {
		g_LogProducer->Produce(msg);
	}
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

	LPVOID pTarget = (LPVOID)(GetWXModuleAddress() + WeChatOffsets::LOGFUN);
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
