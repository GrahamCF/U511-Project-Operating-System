#include <stdint.h>
#include <stddef.h>
#include "stubaddress.h"
#include "terminal.h"
#include "syscall.h"

// Defined in other files
extern "C" void dividebyzero_handler();   // dividebyzero.cpp
extern "C" void keyboard_handler();       // keyboard.cpp
extern "C" void timer_handler();
extern "C" void invalidopcode_handler();
extern "C" void syscall_handler();
extern "C" void doublefault_handler();
// ISR stub declarations
extern "C" void isr0_stub();
extern "C" void isr32_stub();
extern "C" void isr33_stub();
extern "C" void isr6_stub();
extern "C" void isr49_stub();
extern "C" void isr8_stub();

// extern "c" tells the compiler that this should be compiled as assembly code and not c++ code

static inline uint8_t inb(uint16_t port) // return 1 byte from I/O port
{
    unsigned char result; // stores info from I/O port
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port)); // assembly code to read byte
    return result; // return byte
}

static inline void outb(uint16_t port, uint8_t data){ // Reads to end with port being location and data being whats read

   __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

static void remap_pic(void){ // Remaps the PIC to match the IDT entries
    // Start init
    outb(0x20, 0x11); // Master PIC - Command & Slave PIC - Command
    outb(0xA0, 0x11); // IRQ1 for keyboard 

    outb(0x21, 0x20); // Master IRQs for hardware start at vector 32 (IRQ0 - timer)
    outb(0xA1, 0x28); // Slave IRQs start at vector 40 (IRQ8)

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFC); 
    outb(0xA1, 0xFF);
}

// One 8-byte IDT entry
struct __attribute__((packed)) IDT_Entry
{
    uint16_t Offset_low;
    uint16_t Selector;
    uint8_t Reserved;
    uint8_t Flags;
    uint16_t Offset_high;
};

// IDT register format for 32-bit mode
struct __attribute__((packed)) IDT_IDTR
{
    uint16_t limit;
    uint32_t base;
};
    static IDT_Entry idt[256]; // uses the struct of Entry to make parameters for each entry in descriptor table
    static IDT_IDTR idtr; // uses struct of IDTR to make paramaters for limit and base of desciptor table

// calls to lidt to load the IDT and initialize the full interrupt handler table of 256 entries each with 8 bytes of data
// static initilizes everything in the IDT table to 0 

static void set_idt_gate(int entry_number, uint32_t handler_address, uint16_t selector, uint8_t flags){
    idt[entry_number].Offset_low = handler_address & 0xFFFF; // Offset high & low are the address of 32 bit entry
    idt[entry_number].Offset_high = (handler_address >> 16) & 0xFFFF;
    idt[entry_number].Selector = selector; // Points to code in gdt or ldt
    idt[entry_number].Reserved = 0;
    idt[entry_number].Flags = flags; // Present bits = 1, Privelege level = 0 for Kernel, Gate type (E for interrupt)
} // Sets the parameters of each entry

void set_entries(uint16_t cs){
    set_idt_gate(0, (uint32_t)isr0_stub,  cs, 0x8E); // Divide by zero
    set_idt_gate(6, (uint32_t)isr6_stub, cs, 0x8E); // Invalid Opcode
    set_idt_gate(8, (uint32_t)isr8_stub, cs, 0x8E); // Double Fault 
    set_idt_gate(32, (uint32_t)isr32_stub, cs, 0x8E); // Timer
    set_idt_gate(33, (uint32_t)isr33_stub, cs, 0x8E); // Keyboard
    set_idt_gate(49, (uint32_t)isr49_stub, cs, 0xEE); // System call (Its entry number can be anything between 48 - 255)
}

extern "C" __attribute__((naked)) void isr0_stub() { // ISR stub address for interrupt 0 ( Divide by Zero )
    
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "call dividebyzero_handler \n\t" // calls to specific code of handler that will execute 
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"      // return
    );
};

extern "C" __attribute__((naked)) void isr6_stub() { // ISR stub address for interrupt 6 ( invalid opcode )
    
    __asm__ volatile (
        "cld\n\t"
        "call invalidopcode_handler\n\t"
        "1: cli\n\t"
        "hlt\n\t"
        "jmp 1b\n\t"
    );
};

extern "C" __attribute__((naked)) void isr8_stub() { // ISR stub address for interrupt 8 ( double fault )
    
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "call doublefault_handler \n\t" // calls to specific code of handler that will execute 
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"      // return
    );
};

extern "C" __attribute__((naked)) void isr32_stub() { // ISR stub address for interrupt 32 ( Timer )
    
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "cld\n\t"
        "call timer_handler \n\t"
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"         // return
    );
};

extern "C" void __attribute__((naked)) isr33_stub() { // ISR stub address for interrupt 33 ( Keyboard )
   
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "cld\n\t"
        "call keyboard_handler \n\t"
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"        // return
    );
};

extern "C" void __attribute__((naked)) isr49_stub() { // ISR stub address for interrupt 38 ( System Call )
    
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "cld\n\t"
        "call syscall_handler \n\t"
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"      // return
    );
};

extern "C" void idt_initialize()
{
    // Disable interrupts while configuring the IDT and PIC.
    __asm__ volatile ("cli");

    // Remap the PIC so IRQ0-IRQ7 use vectors 0x20-0x27
    // and IRQ8-IRQ15 use vectors 0x28-0x2F.
    remap_pic();

    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));

    // Install specific handlers
    set_entries(cs);

    // Load the IDT register.
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)&idt;
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}