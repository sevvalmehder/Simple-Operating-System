        ; 8080 assembler code
        .hexfile Sort.hex
        .binfile Sort.com
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

memory: ds 50

begin:
    lxi SP, stack                       ; Always initialize the stack pointer
    lxi h, memory                       ; Store the memory address to HL
    mvi c, 0                            ; Inıtialize the counter as a 0

random:
    mvi a, GET_RND                      ; store the OS call code A
    call GTU_OS                         ; call the os
    mov m, b                            ; memory[HL] = B
    inx h                               ; Increase the HL
    inr c                               ; Increase the counter
    mov a, c                            ; A = C
    cpi 50                              ; Compare with 50
    jc random                           ; Jump to loop if A < 50
    jz random                           ; Jump loop if A == 50, then print 50

sort:
    lxi h, memory               ; store the list address in HL
    mvi b, 50              ; for out loop

out_loop:
    dcr b
    jz result                  ; Jump result if A == 0
    mvi c, 0                ; size of list
    lxi h, memory                ; store the list address in HL

compare:
    mov a, m                  ; a = first element
    inx h                     ; increase the memory 
    mov d, m                  ; d = second element
    cmp d                     ; compare a with d
    jc updates                ; jump to updates if A < D
    ; otherwise SWAP!
    mov m, a                   ; store a(the first) content to current location
    dcx h                      ; two step back
    mov m, d                   ; store d(the second) content to back memory block
    ; swap operation is over. Take the memory current location again
    inx h

updates:
    inr c
    mov a, c
    cpi 50
    jc compare                ; if c < 50 go to compare
    ; if c == 0 go back to out loop 
    jmp out_loop

result:
    ; print the result and hlt
    lxi h, memory

print:
    mov b, m
    inx h
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                       ; call the OS
    inr e
    mov a, e
    cpi 50
    jc print
    hlt

