#include <stdint.h>
#include "terminal.h"
#include "syscall.h"

// Defined in other files
extern "C" void dividebyzero_handler();   // dividebyzero.cpp
extern "C" void keyboard_handler();       // keyboard.cpp
extern "C" void timer_handler();
extern "C" void invalidopcode_handler();
extern "C" void syscall_handler();
// ISR stub declarations
extern "C" void isr0_stub();
extern "C" void isr32_stub();
extern "C" void isr33_stub();
extern "C" void isr6_stub();
extern "C" void isr49_stub();

static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Remap the PIC so IRQs land on vectors 32-47, unmask the keyboard only
static void remap_pic()
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);   // Master PIC starts at vector 32
    outb(0xA1, 0x28);   // Slave PIC starts at vector 40

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFC);   // IRQ1 (keyboard) only; timer stays masked
    outb(0xA1, 0xFF);
}

// One 8-byte IDT entry
struct __attribute__((packed)) IDT_Entry
{
    uint16_t Offset_low;
    uint16_t Selector;
    uint8_t  Reserved;
    uint8_t  Flags;
    uint16_t Offset_high;
};

// IDT register format for 32-bit mode
struct __attribute__((packed)) IDT_IDTR
{
    uint16_t limit;
    uint32_t base;
};

static IDT_Entry idt[256];
static IDT_IDTR  idtr;

static void set_idt_gate(int entry_number, uint32_t handler_address,
                         uint16_t selector, uint8_t flags)
{
    idt[entry_number].Offset_low  = handler_address & 0xFFFF;
    idt[entry_number].Offset_high = (handler_address >> 16) & 0xFFFF;
    idt[entry_number].Selector    = selector;
    idt[entry_number].Reserved    = 0;
    idt[entry_number].Flags       = flags;
}

void set_entries(uint16_t cs){
    set_idt_gate(0, (uint32_t)isr0_stub,  cs, 0x8E); // Divide by zero
    set_idt_gate(6, (uint32_t)isr6_stub, cs, 0x8E); // Invalid Opcode
    //set_idt_gate(8, (uint32_t)isr8_stub, 0x08, 0x8E); // Double Fault 
    set_idt_gate(32, (uint32_t)isr32_stub, cs, 0x8E); // Timer
    set_idt_gate(33, (uint32_t)isr33_stub, cs, 0x8E); // Keyboard
    set_idt_gate(49, (uint32_t)isr49_stub, cs, 0xEE); // System call (Its entry number can be anything between 48 - 255)
}

// Catch-all C handler for any vector without a real handler
/*extern "C" void unhandled_interrupt()
{
    terminal_print("UNHANDLED INTERRUPT\n");
    for (;;) __asm__ volatile ("cli; hlt");
}*/

// ---- Naked stubs (AT&T syntax, basic asm only: no operands inside naked) ----

// Catch-all: never returns
/*extern "C" __attribute__((naked)) void default_isr_stub()
{
    __asm__ volatile (
        "pusha\n\t"
        "cld\n\t"
        "call unhandled_interrupt\n\t"
        "1: hlt\n\t"
        "jmp 1b\n\t"
    );
}*/

// Vector 0: divide error. Handler halts forever, so no popa/iret.
extern "C" __attribute__((naked)) void isr0_stub()
{
    __asm__ volatile (
        "cld\n\t"
        "call dividebyzero_handler\n\t"
        "1: hlt\n\t"
        "jmp 1b\n\t"
    );
}

// Vector 33: keyboard (IRQ1). Returns to interrupted code.
extern "C" __attribute__((naked)) void isr33_stub()
{
    __asm__ volatile (
        "pusha\n\t"
        "cld\n\t"
        "call keyboard_handler\n\t"
        "popa\n\t"
        "iret\n\t"
    );
}

extern "C" __attribute__((naked)) void isr32_stub() { // ISR stub address for interrupt 32 ( Timer )
    
    __asm__ __volatile__(

        "pusha\n\t"       // save all general purpose registers
        "cld\n\t"
        "call timer_handler \n\t"
        "popa\n\t"        // restore all general purpose registers
        "iret\n\t"         // return
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
    __asm__ volatile ("cli");

    remap_pic();

    // Use the code segment we're actually running in
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));

    // Everything defaults to the catch-all
    /*for (int i = 0; i < 256; i++)
        set_idt_gate(i, (uint32_t)default_isr_stub, cs, 0x8E);
*/
    set_entries(cs);

    idtr.limit = sizeof(idt) - 1;
    idtr.base  = (uint32_t)&idt;
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}