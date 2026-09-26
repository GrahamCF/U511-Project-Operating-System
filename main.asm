ORG 0x7C00 ;tells the assembler where to load
BITS 16 ; generate 16 bit code

main:
     HLT

halt:
     JMP halt     

TIMES 510-($-$$) DB 0 ;fills binary with 0 bytes to get to 510
DW 0AA55h     ;tells BIOS a storage drive is bootable
