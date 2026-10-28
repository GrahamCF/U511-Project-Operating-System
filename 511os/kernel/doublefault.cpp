#include <stdint.h>
#include <stddef.h>
#include "terminal.h"
#include "doublefault.h"

// CPU exception (Vector entry 6)

struct InterruptFrame {

   uint64_t rip;
   uint64_t cs;
   uint64_t rflags;
   uint64_t rsp;
   uint64_t ss;

};

extern "C" void doublefault_handler(){

            
    
             terminal_print("Double Fault Exception\n");
            
             while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }

};