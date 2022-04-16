#include "sbi.h"

unsigned long TIMECLOCK = 10000000;

unsigned long get_cycles()
{
    unsigned long cycles;
    __asm__ volatile(
        "rdtime %0\n"
        
        : "=r"(cycles)
    );
    return cycles;
}

void clock_set_next_event()
{
    unsigned long next = get_cycles() + TIMECLOCK;

    sbi_ecall(0, 0, next, 0, 0, 0, 0, 0);
}
