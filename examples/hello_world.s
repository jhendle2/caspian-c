global main

section .text

sayHelloWorld:
	push rbp	; Store old stack pointer
	mov  rbp, rsp	; Load stack pointer from function call
	mov  rax, 1	; arg[0]: 1
	mov  rdi, 1	; arg[1]: 1
	mov  rsi, const0	; arg[2]: "Hello, World!", 0x0A
	mov  rdx, const0_len	; arg[3_len]: "Hello, World!", 0x0A size
	syscall
	pop  rbp	; Restore stack pointer
	ret			; Exit function
main:
	push rbp	; Store old stack pointer
	mov  rbp, rsp	; Load stack pointer from function call
	call sayHelloWorld	; call function `sayHelloWorld`
	mov  eax, 0	; Constant load (0)
	pop  rbp	; Restore stack pointer
	ret			; Exit function

section .rodata

section .data
	const0: db "Hello, World!", 0x0A
	const0_len: equ $ - const0


