#include <cstddef> 
#include <x86intrin.h>
#include "terminal.h"
#include <cstdint>
#include "invalidopcode.h"
#include "doublefault.h"

// CPU exception (Vector entry 8)

struct InstructReg { // main registers for double fault exception to report 

   uint32_t eip; // what instruction should be executed

};

InstructReg* reg;

extern "C" void invalidopcode_handler(){
    
    
             terminal_print("Invalid Opcode Exception");

               uint8_t Opcode = *(uint8_t*)reg->eip; // Sets Opcode to 32 bit eip register's current instruction (at that location)

               print_hex(Opcode); // takes the currently executed instruction and takes the first 8 bits to be converted into Hex to represent opcode
            
             while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }

};