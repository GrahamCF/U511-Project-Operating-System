;Kernel Section of the Project
;Current work in progress



ORG 0x8000
BITS 16

;Entry point for the kernel execution should start after main.asm
kernel_entry:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ;install keyboard isr
    mov word [0x9*4], keyboard_isr
    mov word [0x09*4+2], 0x0000

    sti ;should be safe to take interruptions

    mov si, msg_welcome
    call print_string

halt_loop;      ; halt the cpu until the next interruption
    hlt
    jmp halt_loop

;prints a null terminated string via BIOS
;No BIOS call that prints an entire string, print_string is a loop that will print a string
;the kernel will then write "call print_string" instead of manually looping if you want to print something
print_string:
    pusha
    mov ah, 0x0E
.next:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp.next
.done
    popa
    ret


