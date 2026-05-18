extern PushHeadMessageToQueue:proc
PUBLIC HeadHook_Asm

PUBLIC g_HeadNext


.data
g_HeadNext DQ 0

.code
HeadHook_Asm proc
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
	sub rsp, 20h
	mov rcx,[rdx]
	
	call PushHeadMessageToQueue

	add rsp, 20h
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
	
	cmp qword ptr [g_HeadNext], 0
	je short _stuck
	jmp qword ptr [g_HeadNext]

	_stuck:
		int 3
		ret
	HeadHook_Asm endp
	END