#include <stdint.h>
#include "terminal.h" // Terminal functions
#include "keyboard.h" // Keyboard functions
#include "timer.h"
#include "invalidopcode.h"

extern "C" void idt_initialize();

extern "C" void kernel_main() // Use C
{
    terminal_initialize();

    terminal_print("---------------------------------------\n");
    terminal_print("    OPERATING SYSTEMS U511 PROJECT\n");
    terminal_print("        ZACH - GRAHAM - CHRIS\n");
    terminal_print("---------------------------------------\n");

    terminal_print("kernel> ");
    terminal_print("\nKeyboard Test: ");

    idt_initialize();
    timer_init();

    __asm__ volatile ("sti");


    // DIVIDE BY ZERO DEMONSTRATION
    //volatile int zero = 0;
    //volatile int x = 1 / zero;
    //(void)x;

    // Trigger OpCode
    //__asm__ volatile ("ud2");

    // Syscall Example
    __asm__ volatile ("int $0x31");

    // Double fault example
    //__asm__ volatile ("int $0x08");

    while (true) // stop kernel from exiting
    {
        __asm__ volatile ("hlt");
    }
}