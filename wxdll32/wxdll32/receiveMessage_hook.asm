PUBLIC g_ReceiveMessageNext
PUBLIC ReceiveMessageHook_Asm

.data
g_ReceiveMessageNext DQ 0

.code
ReceiveMessageHook_Asm PROC
pushfq
	push rbp
	push rsi
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

	sub rsp,40h


	



	add rsp, 40h
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
	pop rsi
	pop rbp
	popfq

	cmp qword ptr [g_ReceiveMessageNext], 0
	je short _stuck
	jmp qword ptr [g_ReceiveMessageNext]

_stuck:
	int 3
	ret

ReceiveMessageHook_Asm ENDP
END