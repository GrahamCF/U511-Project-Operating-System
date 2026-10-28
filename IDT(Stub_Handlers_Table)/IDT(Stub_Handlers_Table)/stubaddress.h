#ifndef STUBADDRESS_H
#define STUBADDRESS_H

// Stub addresses for each interrupt entry in the table. Stubs will call to handlers

extern "C" __attribute__((naked)) void isr0_stub();
extern "C" __attribute__((naked)) void isr32_stub();
extern "C"  __attribute__((naked)) void isr33_stub();
extern "C" __attribute__((naked)) void isr34_stub(); 
extern "C" __attribute__((naked)) void isr38_stub();

#endif