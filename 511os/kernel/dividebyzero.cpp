#include <stdint.h>
#include <stddef.h>
#include "dividebyzero.h"
#include "terminal.h"

// CPU Exception (Vector entry 0)

extern "C" void dividebyzero_handler(){

   terminal_print("Divide by Zero Exception\n");
   while(true){
      __asm__ __volatile__("cli; hlt"); // Stop CPU from executing instructions
   }
 }

