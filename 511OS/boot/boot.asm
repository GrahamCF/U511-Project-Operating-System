BITS 32 ; tells NASM to generate 32 bit x86 instructions

; GRUB boot information
section .multiboot
align 4

    dd 0x1BADB002 ; identifier for Multiboot-compatible kernel
    dd 0x00000000 ; flags
    dd -(0x1BADB002 + 0x00000000) ; checksum

section .text ; inform assembler and liner instructions are CPU code

global _start ; inform linker of entry point

extern kernel_main ; GRUB transfers control over to kernel

_start:
    cli ; clear interrupts
	
    call kernel_main ; jump from assembly to C++

.hang:
    hlt
    jmp .hang
