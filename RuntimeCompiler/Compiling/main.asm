global _main

section .bss
	c: resd 4
	temp1: resd 4
	temp2: resd 4
	y: resd 4
	x: resd 4

section .data

section .text
printHelloWorld:
	; Store '4' into 'c'
	mov eax, 4
	mov [c], eax

	; Store 'c' into 'temp1'
	mov eax, [c]
	mov [temp1], eax

	; Store '4' into 'temp2'
	mov eax, 4
	mov [temp2], eax

	mov eax, [temp1]
	cmp [temp2], eax

	je STARTLABEL
	jmp ENDLABEL

STARTLABEL:
	; Store 'c' into 'y'
	mov eax, [c]
	mov [y], eax

	mov eax, [c]
	ret
ENDLABEL:
	mov eax, [6]
	ret
_main:
	call printHelloWorld

	; Store 'printHelloWorld' into 'x'
	mov [x], eax


	; Exit Progam
	mov eax, [x]
	add esp, 4	; clear the stack
	ret		; return
