#include <cstddef> 
#include <cstdint>
#include <x86intrin.h>
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

volatile int seconds = 10; // the amount of time before each interruption
volatile int visits = 0; // initilization to 0;
volatile int stop = 0;

static int Currenttime(){

    return visits;
  
}

static void Resettime(){

    visits = 0;
}

static void Stoptime(int state){

    if (state == 1){ // True

        stop = 1;

} else {

        stop = 0;

}

}


extern "C" void timer_handler(){

     if(stop == 0){
    visits++; //keeps track of how many times this interruption occurs
     }
       

    if( ( visits % (seconds * 100) ) == 0 ){ // Timer every 500 times or 5 seconds 
        
        terminal_print("10 seconds has passed");

    }

     outb(0x20, 0x20); // marks the end of interruption thats send to PIC
    terminal_print("Timer Interruption has ended"); 

    
};
