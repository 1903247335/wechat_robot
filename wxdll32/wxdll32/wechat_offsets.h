#pragma once

#include <Windows.h>

// 以下 RVA / 常量随 WeChatWin.dll 版本变化。升级微信后优先只改本文件。
// 使用 static constexpr，在未开 C++17 时也可安全地放在头文件里被多个 .cpp 包含。
namespace wx::offsets {

// DetectWeChatDll：循环次数（hook.cpp 里每次 Sleep(100)）
static constexpr INT64 kPollWeChatDllMaxIterations = 1000;

static constexpr INT64 kLogFunRva = 0x26D9B90;
static constexpr INT64 kMutexStringRva = 0x51C9480;
static constexpr INT64 kReplaceNum = 4;

static constexpr INT64 kQrCodeTargetRva = 0x24D83D0;
static constexpr INT64 kQrCodeRegionSize = 0xE7B;

} // namespace wx::offsets

/*
IDA / x64dbg 摘录（二维码相关，便于下次重定位）：
00007FF8B67983D0 | mov r15,r9
00007FF8B67983D3 | movsxd r12,r8d
00007FF8B67983D6 | movsxd r13,edx
00007FF8B67983D9 | mov rsi,rcx | [[[[rcx+300]+10]+8]]
*/
