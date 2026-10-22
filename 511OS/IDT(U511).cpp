#include <cstddef> 
#include <cstdint>
#include <cstring>
#include <x86intrin.h>

static void inb(uint16_t port){

    uint8_t data;

   __asm__ __volatile__("inb %1, %0" :  "=a"(data) : "Nd"(port));
   return data;

}

static void outb(uint16_t port, unint8_t data){

   __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}


void remap_pic(void){

    // Start init
    outb(0x20, 0x11); // Master PIC - Command & Slave PIC - Command
    outb(0xA0, 0x11); // IRQ1 for keyboard 

    outb(0x21, 0x20); 
    outb(0xA1, 0x28); 

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);

}

// extern "c" tells the compiler that this should be compiled as assembly code and not c++ code
 extern "C" __attribute__((interrupt0)) void isr0_handler(){

 }
 extern "C" __attribute__((interrupt32)) void isr32_handler(){}

 extern "C" __attribute__((interrupt33)) void isr33_handler(){

    const uint16_t scancode = ;

    char scancode_asii[scancode]

 }
 extern "C" __attribute__((interrupt34)) void isr34_handler(){}
 extern "C" __attribute__((interrupt38)) void isr38_handler(){} 

void cli(){  // necessary to disable interrupts setting IF Register Flag to 0 to prevent interrupts while making or modifying the IDT table
    __asm__ __volatile__("cli");
};

void sti(){ // necessary to enable interrupts setting IF Register Flag to 1
    __asm__ __volatile__("sti");
};

typedef struct __attribute__((packed)) IDT_Entry {
    
    uint16_t Reserved;
    uint16_t Offset_low;
    uint16_t Offset_high;
    uint16_t Selector;
    uint8_t Flags;
    
};

typedef struct __attribute__((packed)) IDT_IDTR {

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

    set_idt_gate(0, (uint16_t)isr0_stub,  0x08, 0x8E); // Divide by zero
    set_idt_gate(1, (uint16_t)isr32_stub, 0x08, 0x8E); // Timer
    set_idt_gate(2, (uint16_t)isr33_stub, 0x08, 0x8E); // Keyboard
    set_idt_gate(3, (uint16_t)isr34_stub, 0x08, 0x8E); // Mouse
    set_idt_gate(4, (uint16_t)isr38_stub, 0x08, 0x8E); // System call
}


// ISR Handlers Stubs/entry addresses that correspond to their handlers

extern "C" __attribute__((naked)) void isr0_stub() { // ISR stub address for interrupt 0
    
    __asm__ __volatile__(
        "pushad"       // save all general purpose registers
        "call isr0_handler"
        "popad"        // restore all general purpose registers
        "iret"      // return
    );
};

extern "C" __attribute__((naked)) void isr32_stub() { // ISR stub address for interrupt 32
    
    __asm__ __volatile__(
        "pushad"       // save all general purpose registers
        "call isr32_handler"
        "popad"        // restore all general purpose registers
        "iret"         // return
    );
};

extern "C" __attribute__((naked)) void isr33_stub() { // ISR stub address for interrupt 34
   
    __asm__ __volatile__(
        "pushad"       // save all general purpose registers
        "call isr33_handler"
        "popad"        // restore all general purpose registers
        "iret"        // return
    );
};

extern "C" __attribute__((naked)) void isr34_stub() { // ISR stub address for interrupt 38
   
    __asm__ __volatile__(
       
        "pushad"       // save all general purpose registers
        "call isr34_handler"
        "popad"        // restore all general purpose registers
        "iret"      // return
    );
};


extern "C" __attribute__((naked)) void isr38_stub() { // ISR stub address for interrupt 38
    
    __asm__ __volatile__(
        "pushad"       // save all general purpose registers
        "call isr38_handler"
        "popad"        // restore all general purpose registers
        "iret"        // return
    );
};

// Sets the addresses of the ISR Stubs in the IDT which the stubs will call the handlers as they go 
// Offset high & low point to the entry addresses for the ISR stubs so (uint32_t)&idt + 0 is base for entry 0, and so forth

extern "C" void kernel_main()
{
    load_idtr();
    set_entries();

    // Re-enable hardware interrupts
    __asm__ volatile ("sti");

    while (true)
    {
        __asm__ volatile ("hlt");
    };
};

    