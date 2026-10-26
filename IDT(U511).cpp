#include <cstddef> 
#include <cstdint>
#include <cstring>
#include <x86intrin.h>

static void inb(uint16_t port){ // Reads data at location to start

    uint8_t data;

   __asm__ __volatile__("inb %1, %0" :  "=a"(data) : "Nd"(port));
   
   return data;

}

static void outb(uint16_t port, uint8_t data){ // Reads to end with port being location and data being whats read

   __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Array of chars that map to specific ascii scancodes that will help with keyboard interruptions
static const char lookupTable[] = {

  0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',  0, ' '

};


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

static volatile uint16_t* const VGA_Memory = (0xB8000); // Location of VGA Memory

int row = 0;
int column = 0;

static void print_char(char c, char color = 0x0F){ // Prints color (grey) on black background

 if (c == '\n'){ // newline character

    row++; // increments the row or x by 1 to next line
    column = 0; // starts at column zero of that row to start writing

} if else (c == '\r'){ //return character

    column = 0; // set column to zero and return
    return

 }
  else if(c == '\b'){ // backspace character

    column--; // decrement column since its backspace while row remains

    if (column  == 0){ // If column reaches 0 then row goes back as well.
        
    row--;
    
}


  } else {  // normal characters that get assigned positions to write in VGA_Memory

    int pos = (row * 80) + column; // 80 columns per row + columns
    VGA_Memory[pos] = (char)color << 8 | c; // upper bits for color & char for low bits
    column++ // moves to next column after recieving this character
}

}


// VGA Text Mode 
static void VGA_print(const char* text, uint8_t color){ // a way to traverse a text of characters to print a message

    for(int i = 0; text[i] != '\0'; i++){ 

        print_char(text[i], color);   // Increments the text input by each character to make it
    }
    
    

}

// extern "c" tells the compiler that this should be compiled as assembly code and not c++ code
 extern "C" __attribute__((interrupt0)) void isr0_handler(){

    
    
            VGA_print("Divide by Zero Interruption", 0x0F);
             outb(0x20, 0x20); // marks end of interruption


 }

 static uint32_t visits = 0;
 extern "C" __attribute__((interrupt32)) void isr32_handler(){

    visits++; //keeps track of how many times this interruption occur
    
    while(true){  

    __asm__ __volatile__("hlt"); // tells cpu stop exectuing instructions

    if( visits % 100 == 0 ){ // Timer every 100 times or 1 second 

        VGA_print("Timer Interruption", 0x0F); 

        if (visits >= 100){

            visits = 0; // reset of timer 
        }

    }

    }

     outb(0x20, 0x20); // marks the end of interruption
    




 }

 extern "C" __attribute__((interrupt33)) void isr33_handler(){

    uint8_t scancode = inb(0x60); // tells it to read port (0x60) and keyboard controller will put its data into this port

        if( (scancode & 0x80) == 0){ // 0x80 since its 8 bit scan code, bitwise AND, If highest bit is 0 (key pressed), If highest bit is 1 (key released)

            if(scancode < sizeof(lookupTable)){ // As long as its not greater than the lookup table 

        char C = lookupTable[scancode]; 
    }

        } else {

              outb(0x20, 0x20);  // (Master PIC command, EOI Command) 
            VGA_print("Keyboard Interruption Ended", 0x0F);
        }
 }

 extern "C" __attribute__((interrupt34)) void isr34_handler(){}
 extern "C" __attribute__((interrupt38)) void isr38_handler(){} 

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
        "pusha"       // save all general purpose registers
        "call isr0_handler"
        "popa"        // restore all general purpose registers
        "iret"      // return
    );
};

extern "C" __attribute__((naked)) void isr32_stub() { // ISR stub address for interrupt 32
    
    __asm__ __volatile__(
        "pusha"       // save all general purpose registers
        "call isr32_handler"
        "popa"        // restore all general purpose registers
        "iret"         // return
    );
};

extern "C" __attribute__((naked)) void isr33_stub() { // ISR stub address for interrupt 34
   
    __asm__ __volatile__(
        "pusha"       // save all general purpose registers
        "call isr33_handler"
        "popa"        // restore all general purpose registers
        "iret"        // return
    );
};

extern "C" __attribute__((naked)) void isr34_stub() { // ISR stub address for interrupt 38
   
    __asm__ __volatile__(
       
        "pusha"       // save all general purpose registers
        "call isr34_handler"
        "popa"        // restore all general purpose registers
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

    