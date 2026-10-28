#include <stdint.h>
#include <stddef.h>
#include "terminal.h"
#include "invalidopcode.h"

// CPU exception (Vector entry 6)

extern "C" void invalidopcode_handler(){
   terminal_print("Invalid Opcode Exception");
   /*while(true){
      __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
   }*/
}