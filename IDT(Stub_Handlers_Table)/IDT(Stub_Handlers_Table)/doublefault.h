#ifndef DOUBLEFAULT_H
#define DOUBLEFAULT_H


extern "C" void doublefault_handler();
static void reportRegState();
static void print_hex(uint32_t value);

#endif