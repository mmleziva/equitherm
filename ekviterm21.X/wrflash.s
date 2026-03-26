; Define the address from where the programming has to start
.equ PROG_ADDR, 0x022000
; Load the NVMADR register with the starting programming address
;MOV
 ;#tblpage(PROG_ADDR),W9
;MOV
; #tbloffset(PROG_ADDR),W8
;MOV
; W9,NVMADRU
;MOV
; W8,NVMADR
 .global _wrflash
 _wrflash:
   
push    TBLPAG
MOV W1,NVMADRU
MOV W0,NVMADR
; Setup NVMCON to write 1 row of program memory
MOV #0x4002,W0
MOV W0,NVMCON
; Load the program memory write latches
CALL Load_Write_Latch_Row
; Disable interrupts < priority 7 for next 5 instructions
; Assumes no level 7 peripheral interrupts
DISI #06
; Write the KEY sequence
MOV #0x55,W0
MOV W0,NVMKEY 
MOV #0xAA,W0
MOV W0,NVMKEY
; Start the programming sequence
BSET NVMCON,#15
; Insert two NOPs after programming
NOP
NOP
NOP
NOP
pop    TBLPAG
return
 
Load_Write_Latch_Row:
; This example loads 128 write latches
; W2 points to the address of the data to write to the latches
; Set up a pointer to the first latch location to be written
MOV #0xFA,W0
MOV W0,TBLPAG
MOV #0,W1
; Perform the TBLWT instructions to write the latches
; W2 is incremented in the TBLWTH instruction to point to the
; next instruction location
MOV #128,W3
loop:
TBLWTL.b [W2++], [W1++]
TBLWTL.b [W2++], [W1--]
TBLWTH.b [W2++], [W1]
INC2 W1, W1
DEC W3, W3
BRA NZ, loop
return

