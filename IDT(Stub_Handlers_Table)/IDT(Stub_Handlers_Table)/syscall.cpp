#include <cstddef> 
#include <cstdint>
#include <x86intrin.h>
#include "terminal.h"
#include "syscall.h"
#include "timer.h"
#include "doublefault.h" 

// Vector entry 49


 extern "C"  void syscall_handler(int number, char* message){

   syscall_funct(1);
   syscall_funct(1, "Syscall Function for messages to terminal");

    while(true){

                __asm__ __volatile__("hlt"); // Stop CPU from executing instructions
             }

 }


static void syscall_funct(int number){ // Syscalls with numbers
 
    switch (number) 
 {
    case 1: // Registers State
    
      reportRegState();

    break;

    case 2: // Show Current Timer Ticks

      Currenttime();
    
    break;

    case 3:  // Reset Timer Ticks 

      Resettime();
    
    break;

     case 4:  // Stop Timer

     Stoptime(1);
    
    break;

    case 5: // Resume Timer 

     Stoptime(0);
    
    break;

     case 6:  // exit
        
        return;

    break;
 
    default:  // exit out of menu
    
        return;

    break;
 }

}
    

 static void syscall_funct(int number, const char* message){ //syscalls with numbers and a message

    switch(number) 
 {
    case 1: // Print to terminal
    
    terminal_print(message);

    break;

    default:  // exit out of menu
    
        return;

    break;
 }

 };
