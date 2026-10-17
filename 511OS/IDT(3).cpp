#include <cstdint>
#include <x86intrin.h>

extern "C" void isr0_handler(), isr32_handler(), isr33_handler(), isr34_handler(), isr38_handler(); // Tells the compiler that this is external C code when running 

void cli(){  // necessary to disable interrupts setting IF Register Flag to 0 to prevent interrupts while making or modifying the IDT table
    __asm__ __volatile__("cli");
};

void sti(){ // necessary to enable interrupts setting IF Register Flag to 1
    __asm__ __volatile__("sti");
};

void init_idt(){

    sti()
    static IDT_entry idt[256] // creates an array of 256 entries for the IDT table 
    cli()

} 
// calls to lidt to load the IDT and initialize the full interrupt handler table of 256 entries each with 8 bytes of data
// static initilizes everything in the IDT table to 0 

Struct set_idt_entry(int entry_number, void* handler_stub_address, uint16_t selector, uint8_t flags, uint16_t Offset_high, uint16_t offset_low){
    sti()
    ISR_Stub = handler_stub_address; // address of the ISR hander for the correspondingIDT entry
    entry_number = idt[entry_number]; // get the entry number in the IDT table 
    offset_low = (uint16_t)((uint32_t)handler_stub_address & 0xFFFF); // get lower 16 bits of ISR handler address
    offset_high = (uint16_t)((uint32_t)handler_stub_address >> 16) & 0xFFFF; // get the upper 16 bits of ISR handler address
    selector = 0x08; // code segment selector in GDT for Kernel to run (0x08)
    flags = 0x8E; // Gate bits = Interrupt, Storage = 0, Priveldge level = 0 or kernel only, and Present bit 1
     cli()
} // Fills in one entry in the idt table

Struct idt_register(IDTR* descriptor, IDT_entry* idt){

  uint16_t limit = (sizeof(IDT_entry) * 256) - 1; // size of the IDT Table in bytes - 1', 2 byte limit field in the IDTR register
  uint32_t base = (uint32_t)idt; // linear address of first byte ofthe IDT table in memory, 4-byte base field in the IDTR Register

    descriptor->limit = limit; // stores the limit of the IDT table into descriptor
    descriptor->base = base; // stores the base of the IDT table into descriptor

    lidt(descriptor); // load the IDTR register with the address of the IDT Table using lidt instruction, so the CPU can read it and know where the IDT is located in memory
}

// ISR Handlers
void isr0_handler(); // ISR (Interrupt Service Routine) handler for interrupt 0, divide by zero exception
void isr32_handler(); // ISR handler for interrupt 32, timer interrupt (hardware)
void isr33_handler(); // ISR handler for interrupt 33, Keyboard interrupt (hardware)
void isr34_handler(); // ISR handler for interrupt 34, Mouse interrupt (hardware)
void isr38_handler(); // ISR handler for interrupt 38, System call interrupt (software)


isr0_stub: // ISR stub address for interrupt 0

pushad          // save all general purpose regiseters
call isr0_handler // Call for interrupt handler 0
popad           // restore all general purpose registers
iret // return

isr32_stub: // ISR stub address for interrupt 32

pushad
call isr33_handler //  Call for interrupt handler 33
popad
iret

isr34_stub: // ISR stub address for interrupt 34

pushad
call isr34_handler // call for interrupt handler 34
popad
iret

isr34_stub: // ISR stub address for interrupt 34

pushad
call isr34_handler // call for interrupt handler 34
popad
iret

isr38_stub: // ISR stub address for interrupt 38

pushad 
call isr38_handler // call for interrupt handler 38
popad
iret


Protected:

// Sets the addresses of the ISR Stubs in the IDT which the stubs will call the handlers as they go 
// Offset high & low point to the entry addresses for the ISR stubs so (uint32_t)&idt + 0 is base for entry 0, and so forth

    int_idt()  // intilizes the IDT table with 256 entries each 8 bytes long & entries set the ISR stub address for the corresponding entry
    set_idt_entry(0, (void*)isr0_stub, 0x08, 0x8E, (uint32_t)&idt + 0 , (uint32_t)&idt + 0) // Zero Divide
    set_idt_entry(2, (void*)isr32_stub, 0x08, 0x8E, (uint32_t)&idt + 16 , (uint32_t)&idt + 16) // Timer
    set_idt_entry(3, (void*)isr33_stub, 0x08, 0x8E, (uint32_t)&idt + 32 , (uint32_t)&idt + 32) // Keyboard
    set_idt_entry(4, (void*)isr34_stub, 0x08, 0x8E, (uint32_t)&idt + 48 , (uint32_t)&idt + 48) // Mouse
    set_idt_entry(5, (void*)isr38_stub, 0x08, 0x8E, (uint32_t)&idt + 64 , (uint32_t)&idt + 64) // System Call
    idt_register(&idtr, idt); // Defines address (Descriptor) of IDTR & Loads the IDTR register with the address (limit & base) of the idt table using lidt instruction

    