; Parallel threads with two functions: prints numbers from 0 to 50 and 50 to 100
; Waits until all of them done
        ; 8080 assembler code
        .hexfile 2.hex
        .binfile 2.com
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

tcreate:
    lxi b, f1                 ; Thread will start fonk3
    mvi a, TCreate
    call GTU_OS	                 ; call the OS
    lxi b, f3                 ; Thread will start fonk3
    mvi a, TCreate
    call GTU_OS	                 ; call the OS
    mvi b, 1
    mvi a, TJoin
    call GTU_OS
    mvi b, 2
    mvi a, TJoin
    call GTU_OS
    hlt

f1:
    mov b, a                       ;
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mov a, b                       ; A = B, for increase this value
    adi 1                          ; A = A + 2, add with 2
    cpi 50                         ; Compare with 50
    jz f1                        ; Jump loop if A == 50, then print 50
    jc f1                        ; Jump to loop if A < 50
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS

f3:
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