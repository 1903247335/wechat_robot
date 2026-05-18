; x64 detour for CreateMutexW — 仅保存易失寄存器，保证 call 前 RSP 16 字节对齐
;
; 进入时 RSP mod 16 == 8（从 call CreateMutexW 来的标准状态）
; pushfq + 7*push = 64 字节 -> RSP mod 16 == 8
; sub rsp, 28h -> +40 字节 -> RSP mod 16 == 0，再 call ModifyMutexString

EXTERN ModifyMutexString:PROC


PUBLIC g_CreateMutexWTrampoline
PUBLIC CreateMutexW_Detour_Asm


.DATA
ALIGN 8
g_CreateMutexWTrampoline DQ 0

.CODE

CreateMutexW_Detour_Asm PROC

	pushfq
	push rax
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15


	sub rsp, 28h
	mov rcx, r8
	call ModifyMutexString
	add rsp, 28h

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rax
	popfq

	cmp qword ptr [g_CreateMutexWTrampoline], 0
	je short _stuck
	jmp qword ptr [g_CreateMutexWTrampoline]

_stuck:
	int 3
	ret
CreateMutexW_Detour_Asm ENDP

END
