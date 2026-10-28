#include <cstddef> 
#include <cstdint>
#include <cstring>
#include <x86intrin.h>
#include "stubaddress.h"
#include "terminal.h"
#include "syscall.h"


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

void remap_pic(void){ // Remaps the PIC to match the IDT entries

    // Start init
    outb(0x20, 0x11); // Master PIC - Command & Slave PIC - Command
    outb(0xA0, 0x11); // IRQ1 for keyboard 

    outb(0x21, 0x20); // Master IRQs for hardware start at vector 32 (IRQ0 - timer)
    outb(0xA1, 0x28); // Slave IRQs start at vector 40 (IRQ8)

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFD); 
    outb(0xA1, 0xFF);

}



struct __attribute__((packed)) IDT_Entry {
    
    uint16_t Reserved;
    uint16_t Offset_low;
    uint16_t Offset_high;
    uint8_t ist;
    uint16_t Selector;
    uint8_t Flags;
    
};

struct __attribute__((packed)) IDT_IDTR {

    uint16_t limit;
    uint32_t base;

};


    static IDT_Entry idt[256]; // uses the struct of Entry to make parameters for each entry in descriptor table
    static IDT_IDTR idtr; // uses struct of IDTR to make paramaters for limit and base of desciptor table

// calls to lidt to load the IDT and initialize the full interrupt handler table of 256 entries each with 8 bytes of data
// static initilizes everything in the IDT table to 0 


 void set_idt_gate(int entry_number, uint16_t handler_stub_address, uint16_t selector, uint8_t flags){
   
    idt[entry_number].Reserved = 0;
    idt[entry_number].Offset_low = handler_stub_address & 0xFFFF; // Offset high & low are the address of 32 bit entry
    idt[entry_number].Offset_high = (handler_stub_address >> 16) & 0xFFFF;
    idt[entry_number].Selector = selector; // Points to code in gdt or ldt
    idt[entry_number].Flags = flags; // Present bits = 1, Privelege level = 0 for Kernel, Gate type (E for interrupt)
    

} // Sets the parameters of each entry

void load_idtr(){

    idtr.limit = (sizeof(IDT_Entry) * 256) - 1; // size of the IDT table in bytes - 1
    idtr.base = (uint32_t)&idt; // linear address of first byte of the IDT table in memory
    set_entries();

    __asm__ __volatile__("lidt %0" : : "m"(idtr)); // use lidt instruction to load idt register with address & size of table


}

void set_entries(){

    idt[8].ist = 1; // Switches IST entry to #1 before running the double-fault handler

    set_idt_gate(0, (uint16_t)isr0_stub,  0x08, 0x8E); // Divide by zero
    set_idt_gate(6, (uint16_t)isr6_stub, 0x08, 0x8E); // Invalid Opcode
    set_idt_gate(8, (uint16_t)isr8_stub, 0x08, 0x8E); // Double Fault 
    set_idt_gate(32, (uint16_t)isr32_stub, 0x08, 0x8E); // Timer
    set_idt_gate(33, (uint16_t)isr33_stub, 0x08, 0x8E); // Keyboard
    set_idt_gate(49, (uint16_t)isr49_stub, 0x08, 0xEE); // System call (Its entry number can be anything between 48 - 255)
   
}

extern "C" __attribute__((naked)) void isr0_stub() { // ISR stub address for interrupt 0 ( Divide by Zero )
    
    __asm__ __volatile__(

        "pusha \n\t"       // save all general purpose registers
        "call dividebyzero_handler \n\t" // calls to specific code of handler that will execute 
        "popad \n\t"        // restore all general purpose registers
        "iret \n\t"      // return
    );
};

extern "C" __attribute__((naked)) void isr6_stub() { // ISR stub address for interrupt 6 ( Double Fault )
    
    __asm__ __volatile__(

        "pusha \n\t"       // save all general purpose registers
        "call doublefault_handler \n\t" // calls to specific code of handler that will execute 
        "popad \n\t"        // restore all general purpose registers
        "iret \n\t"      // return
    );
};

extern "C" __attribute__((naked)) void isr8_stub() { // ISR stub address for interrupt 8 ( Invalid Opcode )
    
    __asm__ __volatile__(

        "pusha \n\t"       // save all general purpose registers
        "call invalidopcode_handler \n\t" // calls to specific code of handler that will execute 
        "popad \n\t"        // restore all general purpose registers
        "iret \n\t"      // return
    );
};

extern "C" __attribute__((naked)) void isr32_stub() { // ISR stub address for interrupt 32 ( Timer )
    
    __asm__ __volatile__(

        "pushad \n\t"       // save all general purpose registers
        "call timer_handler \n\t"
        "popad \n\t"        // restore all general purpose registers
        "iretd \n\t"         // return
    );
};

extern "C" void __attribute__((naked)) isr33_stub() { // ISR stub address for interrupt 33 ( Keyboard )
   
    __asm__ __volatile__(

        "pushad \n\t"       // save all general purpose registers
        "call keyboard_handler \n\t"
        "popad \n\t"        // restore all general purpose registers
        "iretd \n\t"        // return
    );
};


extern "C" void __attribute__((naked)) isr49_stub() { // ISR stub address for interrupt 38 ( System Call )
    
    __asm__ __volatile__(

        "pushad \n\t"       // save all general purpose registers
        "call syscall_handler \n\t"
        "popad \n\t"        // restore all general purpose registers
        "iretd \n\t"      // return
    );
};

// Sets the addresses of the ISR Stubs in the IDT which the stubs will call the handlers as they go 

extern "C" void kernel_main() // Use C
{

    load_idtr();
    set_entries();

    terminal_initialize();

    terminal_print(" OPERATING SYSTEMS U511 PROJECT");
    terminal_putchar('\n');

    terminal_print("ZACH - GRAHAM - CHRIS");
    terminal_putchar('\n');

    terminal_print("511OS> ");
    terminal_putchar('\n');

    isr49_stub(); // Syscall functions

    while (true) // stop kernel from exiting
    {
        __asm__ volatile ("hlt");
    }
}

    