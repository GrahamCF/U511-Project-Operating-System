#ifndef TIMER_H
#define TIMER_H


void timer_init();

extern "C" void timer_handler();
void Currenttime();
void Stoptime(int state);
void Resettime();

#endif