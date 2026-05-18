/*
 *  trampoline.h — 蹦床与指令补丁结构（源自 MinHook，BSD 许可）
 *  Copyright (C) 2009-2017 Tsuda Kageyu. All rights reserved.
 *  见: https://github.com/TsudaKageyu/minhook
 */
#pragma once

#include <Windows.h>

#pragma pack(push, 1)

typedef struct _JMP_REL_SHORT
{
	UINT8  opcode;
	INT8   operand;
} JMP_REL_SHORT, * PJMP_REL_SHORT;

typedef struct _JMP_REL
{
	UINT8  opcode;
	INT32  operand;
} JMP_REL, * PJMP_REL;

typedef struct _JMP_ABS
{
	UINT8  opcode0;
	UINT8  opcode1;
	UINT32 dummy;
	UINT64 address;
} JMP_ABS, * PJMP_ABS;

typedef struct _CALL_ABS
{
	UINT8  opcode0;
	UINT8  opcode1;
	UINT32 dummy0;
	UINT8  dummy1;
	UINT8  dummy2;
	UINT64 address;
} CALL_ABS;

typedef struct _JCC_REL
{
	UINT8  opcode0;
	UINT8  opcode1;
	INT32  operand;
} JCC_REL;

typedef struct _JCC_ABS
{
	UINT8  opcode;
	UINT8  dummy0;
	UINT8  dummy1;
	UINT8  dummy2;
	UINT32 dummy3;
	UINT64 address;
} JCC_ABS;

#pragma pack(pop)

typedef struct _TRAMPOLINE
{
	LPVOID pTarget;
	LPVOID pDetour;
	LPVOID pTrampoline;
	LPVOID pRelay;
	BOOL   patchAbove;
	UINT   nIP;
	UINT8  oldIPs[32];
	UINT8  newIPs[32];
} TRAMPOLINE, * PTRAMPOLINE;

BOOL CreateTrampolineFunction(PTRAMPOLINE ct);
