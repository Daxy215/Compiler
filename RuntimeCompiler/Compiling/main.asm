global _main

section .bss
	c: resd 4
	temp1: resd 4
	temp2: resd 4
	temp3: resd 4
	y: resd 4
	total: resd 4
	temp7: resd 4
	temp8: resd 4
	temp9: resd 4
	temp6: resd 4
	temp10: resd 4
	temp11: resd 4
	temp4: resd 4
	temp5: resd 4
	temp12: resd 4
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

	; Store '4' into 'temp7'
	mov eax, 4
	mov [temp7], eax

	; Store '4' into 'temp8'
	mov eax, 4
	mov [temp8], eax

	; Add [temp7] by [temp8]
	mov eax, [temp7]
	mov ebx, [temp8]
	xor edx, edx		; Clear the high-order bits in edx
	div ebx		; Divide ebx by eax

	; Store results in [temp9]
	mov [temp9], eax

	; Store 'temp9' into 'temp6'
	mov eax, [temp9]
	mov [temp6], eax

	; Store '8' into 'temp10'
	mov eax, 8
	mov [temp10], eax

	; Add [temp6] by [temp10]
	mov eax, [temp6]
	mov ebx, [temp10]
	mul ebx		; Multiply eax by ebx

	; Store results in [temp11]
	mov [temp11], eax

	; Store '2' into 'temp4'
	mov eax, 2
	mov [temp4], eax

	; Store 'temp11' into 'temp5'
	mov eax, [temp11]
	mov [temp5], eax

	; Add [temp4] by [temp5]
	mov eax, [temp4]
	mov ebx, [temp5]
	add eax, ebx		; Add ebx to eax

	; Store results in [temp12]
	mov [temp12], eax

	; Store 'temp12' into 'total'
	mov eax, [temp12]
	mov [total], eax

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
