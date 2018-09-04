; Creates a thread that print numbers from 0 to 50, waits until it is done
; then creates another thread that adds number from 1 to 10 and print the result
; waits until it is done then creates another thread that print numbers from 50 to 100

        ; 8080 assembler code
        .hexfile 1.hex
        .binfile 1.com
        ; try "hex" for downloading in hex format
        .download bin
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 1
PRINT_MEM	equ 2
READ_B		equ 3
READ_MEM	equ 4
PRINT_STR	equ 5
READ_STR	equ 6
GET_RND     equ 7
TExit       equ 8
TJoin       equ 9
TYield      equ 10
TCreate     equ 11

	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE

	; This program prints integers from 0 to 50 by steps of 2 on the screen.
	; Each number will be printed on a new line.
sum	ds 2 ; will keep the sum

begin:
    LXI SP, stack                  ; Always initialize the stack pointer
    lxi b, f1
    mvi a, TCreate	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mvi b, 1
    mvi a, TJoin
    call GTU_OS
    lxi b, f3
    mvi a, TCreate	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mvi b, 2
    mvi a, TJoin
    call GTU_OS
    lxi b, f2
    mvi a, TCreate	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mvi b, 3
    mvi a, TJoin
    call GTU_OS
    hlt

f1:
    jmp loop
loop:
    mov b, a                       ;
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mov a, b                       ; A = B, for increase this value
    adi 1                          ; A = A + 2, add with 2
    cpi 50                         ; Compare with 50
    jz loop                        ; Jump loop if A == 50, then print 50
    jc loop                        ; Jump to loop if A < 50
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS

f3:
    mvi c, 10	; init C with 10
    mvi a, 0	; A = 0
    jmp loop3

loop3:
    ADD c		; A = A + C
    DCR c		; --C
    JNZ loop3	; goto loop if C!=0
    STA SUM		; SUM = A
    LDA SUM		; A = SUM
            ; Now we will call the OS to print the value of sum
    MOV B, A	; B = A
    MVI A, PRINT_B	; store the OS call code to A
    call GTU_OS	; call the OS
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS

f2:
    mvi a, 50
    jmp loop2

loop2:
    mov b, a                       ;
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mov a, b                       ; A = B, for increase this value
    adi 1                          ; A = A + 2, add with 2
    cpi 100                         ; Compare with 50
    jz loop2                        ; Jump loop if A == 50, then print 50
    jc loop2                        ; Jump to loop if A < 50
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS


