global _main

section .bss

section .data

section .text

_main:
	; Function prologue
	push ebp
	mov ebp, esp
	; Parameters
	; Function body 

	; Exit Progam
	mov eax, [0]
	add esp, 4	; clear the stack
	ret		; return
