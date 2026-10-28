#include <cstddef> 
#include <cstdint>
#include <x86intrin.h>
#include "dividebyzero.h"
#include "terminal.h"

// CPU Exception (Vector entry 0)

extern "C" void dividebyzero_handler(){

    
             terminal_print("Divide by Zero Exception");
            
             while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }
            
            


 };

