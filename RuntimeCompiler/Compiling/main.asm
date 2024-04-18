global _main

section .data

section .bss

section .text


printHelloWorld:
	; Parameters
	; Parameter int g
	mov eax, [ebp + 8]

	; Parameter int i
	mov ebx, [ebp + 12]

	; Function body

	; Allocate space for c
	sub esp, 4
	mov dword [ebp - 8], 0

	; Store '4' into 'c'
	mov eax, 4
	mov [ebp - 8], eax

	; Allocate space for temp1
	sub esp, 4
	mov dword [ebp - 12], 0

	; Store 'c' into 'temp1'
	mov eax, [ebp - 8]
	mov [ebp - 12], eax

	; Allocate space for temp2
	sub esp, 4
	mov dword [ebp - 16], 0

	; Store '4' into 'temp2'
	mov eax, 4
	mov [ebp - 16], eax

	; Allocate space for temp3
	sub esp, 4
	mov dword [ebp - 20], 0

	mov eax, [ebp - 12]
	cmp [ebp - 16], eax

	; IF_STATEMENT
	je START1LABEL
	jmp END1LABEL

START1LABEL:
	; Allocate space for y
	sub esp, 4
	mov dword [ebp - 24], 0

	; Store 'c' into 'y'
	mov eax, [ebp - 8]
	mov [ebp - 24], eax

	mov eax, [ebp - 8]
	ret		; returns/leaves out of the function

END1LABEL:
	; Allocate space for temp4
	sub esp, 4
	mov dword [ebp - 28], 0

	; Store 'c' into 'temp4'
	mov eax, [ebp - 8]
	mov [ebp - 28], eax

	; Allocate space for temp5
	sub esp, 4
	mov dword [ebp - 32], 0

	; Store '6' into 'temp5'
	mov eax, 6
	mov [ebp - 32], eax

	; Allocate space for temp6
	sub esp, 4
	mov dword [ebp - 36], 0

	mov eax, [ebp - 28]
	cmp [ebp - 32], eax

	; ELSEIF_STATEMENT
	je START2LABEL
	jmp END2LABEL

START2LABEL:
	; Allocate space for total
	sub esp, 4
	mov dword [ebp - 40], 0

	; Allocate space for temp10
	sub esp, 4
	mov dword [ebp - 44], 0

	; Store '4' into 'temp10'
	mov eax, 4
	mov [ebp - 44], eax

	; Allocate space for temp11
	sub esp, 4
	mov dword [ebp - 48], 0

	; Store '4' into 'temp11'
	mov eax, 4
	mov [ebp - 48], eax

	; Allocate space for temp12
	sub esp, 4
	mov dword [ebp - 52], 0

	; Add temp10 by temp11
	mov eax, [ebp - 44]
	mov ebx, [ebp - 48]
	xor edx, edx		; Clear the high-order bits in edx
	div ebx				; Divide ebx by eax

	; Store results in temp12
	mov [ebp - 52], eax

	; Allocate space for temp9
	sub esp, 4
	mov dword [ebp - 56], 0

	; Store 'temp12' into 'temp9'
	mov eax, [ebp - 52]
	mov [ebp - 56], eax

	; Allocate space for temp13
	sub esp, 4
	mov dword [ebp - 60], 0

	; Store '8' into 'temp13'
	mov eax, 8
	mov [ebp - 60], eax

	; Allocate space for temp14
	sub esp, 4
	mov dword [ebp - 64], 0

	; Add temp9 by temp13
	mov eax, [ebp - 56]
	mov ebx, [ebp - 60]
	mul ebx				; Multiply eax by ebx

	; Store results in temp14
	mov [ebp - 64], eax

	; Allocate space for temp7
	sub esp, 4
	mov dword [ebp - 68], 0

	; Store '2' into 'temp7'
	mov eax, 2
	mov [ebp - 68], eax

	; Allocate space for temp8
	sub esp, 4
	mov dword [ebp - 72], 0

	; Store 'temp14' into 'temp8'
	mov eax, [ebp - 64]
	mov [ebp - 72], eax

	; Allocate space for temp15
	sub esp, 4
	mov dword [ebp - 76], 0

	; Add temp7 by temp8
	mov eax, [ebp - 68]
	mov ebx, [ebp - 72]
	add eax, ebx				; Add ebx to eax

	; Store results in temp15
	mov [ebp - 76], eax

	; Store 'temp15' into 'total'
	mov eax, [ebp - 76]
	mov [ebp - 40], eax

	mov eax, [ebp - 40]
	ret		; returns/leaves out of the function

END2LABEL:
	; ELSE_STATEMENT
	je START3LABEL
	jmp END3LABEL

START3LABEL:
	; Allocate space for l
	sub esp, 4
	mov dword [ebp - 80], 0

	; Store 'g' into 'l'
	mov eax, [ebp + 8]
	mov [ebp - 80], eax

	mov eax, [ebp - 80]
	ret		; returns/leaves out of the function

END3LABEL:
	mov eax, 6
	ret		; returns/leaves out of the function

_main:
	; Function prologue
	push ebp
	mov ebp, esp

	; Parameters
	; Function body

	; Allocate space for x
	sub esp, 4
	mov dword [ebp], 0

	; Parameter value 5
	push 5
	; Parameter value 8
	push 8
	; Function call
	call printHelloWorld

	; Function prologue for printHelloWorld
	push ebp
	mov ebp, esp

	; Clean up printHelloWorld 
	add esp, 84

	; Exit Progam
	mov eax, [ebp]
	add esp, 4	; clear the stack
	ret		; returns/leaves out of the function
