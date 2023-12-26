
section .data
    t2 dd 3            ; Define a variable to hold 3
    t3 dd 2            ; Define a variable to hold 2
    t4 dd 0            ; Define a variable to hold the result of t2 * t3
    t0 dd 5            ; Define a variable to hold 5
    t1 dd 0            ; Define a variable to hold the result of t4
    t5 dd 0            ; Define a variable to hold the result of t0 + t1

section .text
    global _start

_start:
    ; Load values into registers
    mov eax, [t2]       ; Move the value of 't2' into eax
    mov ebx, [t3]       ; Move the value of 't3' into ebx
    
    ; Perform the operation t4 = t2 * t3
    imul eax, ebx       ; Multiply eax (t2) by ebx (t3), result in eax
    mov [t4], eax       ; Store the result of the multiplication in t4
    
    ; Load value from t4 into ebx (t1 = t4)
    mov ebx, [t4]       ; Move the value of 't4' into ebx (t1 = t4)
    
    ; Load values into registers
    mov eax, [t0]       ; Move the value of 't0' into eax
    
    ; Perform the operation t5 = t0 + t1
    add eax, ebx        ; Add eax (t0) to ebx (t1), result in eax
    mov [t5], eax       ; Store the result of the addition in t5
    
    ; eax now holds the result (t5)
    
    ; If you want to exit cleanly:
    mov eax, 1          ; syscall for exit
    xor ebx, ebx        ; exit code 0
    int 0x80            ; make the syscall
    