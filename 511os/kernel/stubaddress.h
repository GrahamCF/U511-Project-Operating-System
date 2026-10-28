#ifndef STUBADDRESS_H
#define STUBADDRESS_H

// Stub addresses for each interrupt entry in the table. Stubs will call to handlers

extern "C" {
    void isr0_stub();
    void isr6_stub();
    void isr8_stub();
    void isr32_stub();
    void isr33_stub();
    void isr49_stub();
    void default_isr_stub();
}
#endif