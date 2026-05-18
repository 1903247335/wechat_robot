EXTERN wsprintfW:PROC
EXTERN PushQrcodeMessageToQueue:PROC
EXTERN MySafeLog:PROC


PUBLIC g_QrcodeNext

.DATA
g_QrcodeNext DQ 0
fmt  dw 'h','o','o','k',' ','q','r','c','o', 'd','e','=','%','p', 13,10,0   
buf  DW 256 DUP(0)
QRCODESIZE EQU 0E6Bh

.code
QrcodeHook_Asm proc
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

	mov r15,rcx
	lea rcx, buf          ; 参数 1 (RCX): lpOut
	lea rdx, fmt            ; 参数 2 (RDX): lpFmt

	
	push rdx
	push rcx
	mov rcx,r15
	mov rdx, [rcx+300h]
	mov rsi, [rdx+10h]             ; 原始 RCX
	mov r14, [rsi+10h]
	mov rcx, [rsi+8h]            ; 原始 RDX

	mov r8, [rcx]             ; 参数 3 (R8):  对应 %p (原始 RCX)
	
	pop rcx
	pop rdx
	

	;call wsprintfW

	;lea rcx, buf
    ;call MySafeLog

	mov rcx,r8
	mov rdx, r14
	call PushQrcodeMessageToQueue

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

	cmp qword ptr [g_QrcodeNext], 0
	je short _stuck
	jmp qword ptr [g_QrcodeNext]

_stuck:
	int 3
	ret

QrcodeHook_Asm endp

END
