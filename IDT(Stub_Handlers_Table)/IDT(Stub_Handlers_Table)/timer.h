#ifndef TIMER_H
#define TIMER_H



extern "C" void timer_handler();
static int Currenttime();
static void Stoptime(int state);
static void Resettime();

#endif