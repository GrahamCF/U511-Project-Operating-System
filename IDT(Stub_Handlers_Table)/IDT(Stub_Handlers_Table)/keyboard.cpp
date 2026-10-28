#include <stdint.h>
#include "keyboard.h"
#include "terminal.h"

// Hardware Interruption (Vector entry 33 with mapped IRQ being IRQ1)

static inline uint8_t inb(uint16_t port) // return 1 byte from I/O port
{
    unsigned char result; // stores info from I/O port
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port)); // assembly code to read byte
    return result; // return byte
}

static inline void outb(uint16_t port, uint8_t data){ // Reads to end with port being location and data being whats read

   __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// Scan code set 1 lookup table for standard key presses, used to translate keyboard scan into characters using ascii
static const char scancode_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',  0, ' '
}; // does not account for shift key being held


// Basic lowercase keyboard handler
extern "C" void keyboard_handler(){

    uint8_t scancode = inb(0x60); // tells it to read port (0x60) and keyboard controller will put its data into this port

        if( (scancode & 0x80) == 0){ // 0x80 since its 8 bit scan code, bitwise AND, If highest bit is 0 (key pressed), If highest bit is 1 (key released)

            if(scancode < sizeof(scancode_ascii)){ // As long as its not greater than the lookup table 

        char C = scancode_ascii[scancode]; 
    }

        } else {

              outb(0x20, 0x20);  // (Master PIC command, EOI Command) 
            terminal_print("Keyboard Interruption Ended");
        }

 }