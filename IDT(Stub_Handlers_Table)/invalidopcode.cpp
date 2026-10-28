#include <cstddef> 
#include <x86intrin.h>
#include "terminal.h"
#include <cstdint>
#include "invalidopcode.h"

// CPU exception (Vector entry 8)

extern "C" void invalidopcode_handler(){
    
    
             terminal_print("Invalid Opcode Exception");
            
             while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }

}