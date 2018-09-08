       ; 8080 assembler code
        .hexfile p4.hex
        .binfile p4.com
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
FileCreate  equ 12
FileClose   equ 13
FileOpen    equ 14
FileRead    equ 15
FileWrite   equ 16
FileSeek    equ 17
DirRead     equ 18

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


	nums: dw '1 2 3 4 5 6 7 8 9 10', 00AH, 00H;

	filename: ds 1024 ;
	buffer:   ds 1024 ;

begin:
    lxi SP, stack                  ; Always initialize the stack pointer
    lxi b, filename                ; Put the filename to memory
    mvi a, READ_STR                ; Take a filename at the memory[BC]
	call GTU_OS        

	lxi b, filename                ; Put the filename to memory
	mvi a, fileOpen                ; Open the created file
	call GTU_OS

	mov e, b                       ; E = opened file handle ID (in register B)
	mvi d, 140                     ; d holds the number of bytes, 20 byte with whitespace

write:
	lxi b, nums                    ; Put the numbers to memory
	mvi a, FileWrite               ; Write the buffer content to the file
	call GTU_OS
	
finish:
	mov b, e
	mvi a, FileClose               ; Close the file that has a handle ID in register b
	call GTU_OS
	hlt                            ; hlt the program
    