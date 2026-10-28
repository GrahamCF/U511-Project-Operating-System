#ifndef SYSCALL_H
#define SYSCALL_H

 extern "C" void syscall_handler();
 static void syscall_funct(int number);
 static void syscall_funct(int number, char* message);


#endif