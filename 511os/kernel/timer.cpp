#include <stdint.h>
#include <stddef.h>
#include "timer.h"
#include "terminal.h"

// Hardware Interrupt (Vector entry 32 with mapped IRQ being IRQ0)

static inline uint8_t inb(uint16_t port) // return 1 byte from I/O port
{
    unsigned char result; // stores info from I/O port
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port)); // assembly code to read byte
    return result; // return byte
}

static inline void outb(uint16_t port, uint8_t data){ // Reads to end with port being location and data being whats read

   __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

volatile uint32_t seconds = 10; // the amount of time before each interruption
volatile uint32_t visits = 0; // initilization to 0;
volatile uint32_t stop = 0;

void Currenttime(){
    if (visits > 0 && (visits % (seconds * 100)) == 0){ // Timer every 500 times or 5 seconds  
        terminal_print("\n10 seconds has passed\n");
    }
}

void Resettime(){

    visits = 0;
}

void Stoptime(int state){

    if (state == 1){ // True

        stop = 1;

} else {

        stop = 0;

}

}

void timer_init()
{
    uint16_t divisor = 1193182 / 100;
    outb(0x43, 0x36);                    // channel 0, lo/hi byte, square wave
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

extern "C" void timer_handler(){

     if(stop == 0){
        visits = visits + 1; //keeps track of how many times this interruption occurs
     }
    
    Currenttime();

     outb(0x20, 0x20); // marks the end of interruption thats send to PIC
}
