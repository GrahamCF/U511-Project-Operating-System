#include <cstddef> 
#include <stdint.h>
#include <x86intrin.h>
#include "terminal.h"
#include "doublefault.h"
#include <cstdint>

// CPU exception (Vector entry 6)

struct Registers { // main registers for double fault exception to report 

   uint32_t eip; // what instruction should it execute
   uint32_t cs; // what code segment is running 
   uint32_t eflags; // collection of flags with varied bits (Bits 0 = Carry Flag, Bit 6 = zero flag, Bits 7 = Sign Flag, Bits 9 = interrupt Flag )

};

   Registers* doublefault; // pointer to register frame called doublefault


static void reportRegState(){

   terminal_print(" EIP: \n");
      print_hex(doublefault->eip);// prints hex of register eip

      terminal_print(" OPCode at (EIP): \n");
      uint8_t Opcode = *(uint8_t*)doublefault->eip; // Sets Opcode to 32 bit eip register's current instruction (at that location)
       print_hex(Opcode);

   terminal_print(" CS: \n");
            print_hex(doublefault->cs); // prints hex of register cs

   terminal_print(" EFLAGS: \n");
            print_hex(doublefault->eflags); // prints hex of registers cs

}
  
static void print_hex(uint32_t value){

const char* hex = "012345678910ABCDEF"; // Hex digits 

   terminal_print("0x"); // start for hex digits

   for (int i = 28; i >= 0; i -= 4){ // take every 4 bits which represents hexidecimals

      uint8_t digit = (value >> i) & 0xF; // shift value to the right i number of bits and do bitwise addition for digit that corresponds to a hex value in the hex array
      terminal_putchar(hex[digit]); // prints the char of each hex digit as it goes through each 4 bits 


   }


}

extern "C" void doublefault_handler(){

            
             terminal_print("Double Fault Exception \n");

             terminal_print("Current Register States:");

            terminal_print(" EIP: \n");
            print_hex(doublefault->eip); // prints hex of register eip

            terminal_print(" CS: \n");
            print_hex(doublefault->cs); // prints hex of register cs

            terminal_print(" EFLAGS: \n");
            print_hex(doublefault->eflags); // prints hex of register eflags
            
             while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }

};