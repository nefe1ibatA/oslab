#include "sbi.h"

unsigned long TIMECLOCK = 20000000;

unsigned long get_cycles()
{
    unsigned long cycles;
    // call rdtime
    __asm__ volatile(
        "rdtime %0\n"
        
        : "=r"(cycles)
    );
    return cycles;
}

void clock_set_next_event()
{
    unsigned long next = get_cycles() + TIMECLOCK;
    // call sbi_ecall
    sbi_ecall(0, 0, next, 0, 0, 0, 0, 0);
}
