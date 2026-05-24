EXTERN HandleLogHook:PROC

PUBLIC WeChatLogHook_Asm
PUBLIC g_LogBack

.DATA
ALIGN 8
g_LogBack DQ 0
g_Count DQ 0
.CODE
WeChatLogHook_Asm  proc
	push rbp
	push rbx
	lea rbx, [rsp+10h]          ; rbx = 进入 hook 时的原始 rsp

	; 先保存 callee-save，再借用 r12-r15 存参数
	push r12
	push r13
	push r14
	push r15

	mov r15, rcx                ; level
	mov r14, rdx                ; file
	mov r13, r8                 ; line
	mov r12, r9                 ; func

	pushfq
	push rax
	push rcx
	push rdx
	push r8
	push r9
	push r10
	push r11

	; shadow(0x20) + 第5参数槽(0x8)，call 前 RSP 16 字节对齐
	sub rsp, 30h

	mov rcx, [rbx+30h]               ;fmt
	mov rdx, [g_Count]                
	mov r8,r12
	call HandleLogHook
	mov rax,[g_Count] 
	inc rax
	mov [g_Count],rax

	add rsp, 30h

	pop r11
	pop r10
	pop r9
	pop r8
	pop rdx
	pop rcx
	pop rax
	popfq

	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	jmp  qword ptr [g_LogBack]

WeChatLogHook_Asm ENDP

END
