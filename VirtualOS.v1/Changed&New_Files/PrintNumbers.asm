        ; 8080 assembler code
        .hexfile PrintNumbers.hex
        .binfile PrintNumbers.com
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
    mvi e, 2

loop:
    mov b, a                       ; B = C, for printing the content of B
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mov a, b                       ; A = B, for increase this value
    add e                          ; A = A + E, add with 2
    cpi 50                         ; Compare with 50
    jz loop                        ; Jump loop if A == 50, then print 50
    jc loop                        ; Jump to loop if A < 50
    hlt                            ; Otherwise, halt the program