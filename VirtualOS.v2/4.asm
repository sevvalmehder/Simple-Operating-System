; Created two parallel thread for two function:
; 1. Produces 50 random bytes and sort increasing order then print them 
; 2. Produces 50 random bytes and sort them. Then find the 1th bytes 
; using binary search
        ; 8080 assembler code
        .hexfile 4.hex
        .binfile 4.com
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

memory: ds 50
memory: ds 50
message: dw ' No such element',00AH,00H ; null terminated string
search_element1: ds 1
search_element110: ds 1
search_element140: ds 1

begin:
    LXI SP, stack                  ; Always initialize the stack pointer

tcreate:
    lxi b, f4                 ; Thread will start fonk3
    mvi a, TCreate
    call GTU_OS	                 ; call the OS
    lxi b, f5                 ; Thread will start fonk3
    mvi a, TCreate
    call GTU_OS	                 ; call the OS
    mvi b, 1
    mvi a, TJoin
    call GTU_OS
    mvi b, 2
    mvi a, TJoin
    call GTU_OS
    hlt

f4:
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
    inx h                     ; increase the memory 2 times
    mov d, m                  ; d = second element
    cmp d                     ; compare a with d
    jc updates                ; jump to updates if A < D
    ; otherwise SWAP!
    mov m, a                   ; store a(the first) content to current location
    dcx h                      ; two step back
    mov m, d                   ; store d(the second) contetn to 2 step back
    ; swap operation is over. Take the memory current location again
    inx h

updates:
    inr c
    mov a, c
    cpi 50
    jc compare                ; if c != 0 go to compare
    ; if c == 0 decrease the out loop size
    jmp out_loop

result:
    ; print the result and hlt
    lxi h, memory

print:
    mov b, m
    inx h
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    inr e
    mov a, e
    cpi 50
    jc print
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS




f5:
    lxi h, memory                       ; Store the memory address to HL
    mvi c, 0                            ; Inıtialize the counter as a 0

random2:
    mvi a, GET_RND                      ; store the OS call code A
    call GTU_OS                         ; call the os
    mov m, b                            ; memory[HL] = B
    inx h                               ; Increase the HL
    inr c                               ; Increase the counter
    mov a, c                            ; A = C
    cpi 50                              ; compare2 with 50
    jc random2                           ; Jump to loop if A < 50
    jz random2                           ; Jump loop if A == 50, then print 50

sort2:
    lxi h, memory               ; store the list address in HL
    mvi b, 50                   ; for out loop

out_loop2:
    dcr b
    jz search                  ; Jump search
    mvi c, 0                   ; size of list
    lxi h, memory                ; store the list address in HL

compare2:
    mov a, m                  ; a = first element
    inx h                     ; increase the memory
    mov d, m                  ; d = second element
    cmp d                     ; compare2 a with d
    jc updates2                ; jump to updates2 if A < D
    ; otherwise SWAP!
    mov m, a                   ; store a(the first) content to current location
    dcx h                      ; two step back
    mov m, d                   ; store d(the second) content to back memory block
    ; swap operation is over. Take the memory current location again
    inx h

updates2:
    inr c
    mov a, c
    cpi 50
    jc compare2                ; if c < 50 go to compare2
    ; if c == 0 go back to out loop
    jmp out_loop2

search:
    lxi h, memory
    mov d, l                       ; d = l for keep the beginning of the list
    mov a, m                       ; a = b
    sta search_element1
    mvi e, 50                      ; size of the array

median:
    mov a,e                        ; move the size to accumulator
    ; if the array size 0
    cpi 0
    ; Division with 2
    rrc                            ; rotate the accumulator
    ani 127                       ; and with 7f to change the most significant bit as a zero
    cpi 1
    jz handle                      ; if the median is 1, handle the recursive error
    adi 1
    mov e, a                     ; change the new size as a old_size/2
    mvi c, 0                       ; Counter
    jmp loop

handle:
    mvi e, 0
    jmp median

loop:
    mov b, m                       ; Take the next element
    inx h
    inr c                          ; counter += 1
    mov a, c
    cmp e                          ; compare2 size with counter
    jz  median_here                ; if counter == size, this element is median
    jmp loop

median_here:
    ; compare2 the element to searhc(in register d) with the median (in register b)
    lda search_element1             ; a = search_element1
    cmp b                          ; compare2 with median
    jc left_part                    ; if A < B go with the left part
    jz final
    mov d, l                      ; d = L for keep the beginning of list
    jmp median

left_part:
    mov l, d                   ; take the first element address
    jmp median

final:
    dcx h
    mov b, l                       ; b = least significant bits of memory address to found element
    mvi a, PRINT_B	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS
    mvi a, TExit	               ; store the OS call code to A
    call GTU_OS	                   ; call the OS