; Creates 10 parallel threads that prints number from 50 to 100
; Waits until all of them done.
        ; 8080 assembler code
        .hexfile 3.hex
        .binfile 3.com
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

begin:
    LXI SP, stack                  ; Always initialize the stack pointer

tcreate_loop:
    lxi b, fonk3                 ; Thread will start fonk3
    mvi a, TCreate
    call GTU_OS	                 ; call the OS
    inr c                        ; Counter += 1
    mov a, c
    cpi 10                       ; Compare with 10
    jc tcreate_loop               ; Jump to loop if A < 10
    mvi b, 1

tjoin_loop:
    mvi a, TJoin
    call GTU_OS
    inr b                         ; b = b + 1
    mov a, b
    cpi 11                        ; Compare with 10
    jc tcreate_loop               ; Jump to loop if A < 10
    hlt

fonk3:
    mvi b, 50                      ; b = 50

fonk3_cont:
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    inr b                          ; B = B + 1
    mov a, b
    cpi 100                        ; Compare with 100
    jc fonk3_cont                        ; Jump to loop if A < 100
    jz fonk3_cont                        ; Jump loop if A == 100, then print 100
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
