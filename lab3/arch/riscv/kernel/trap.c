#include "printk.h"

#define supervisor_timer_interrupt 0x8000000000000005

extern void clock_set_next_event();
extern void do_timer();

void trap_handler(unsigned long scause, unsigned long sepc) {
    if (scause == supervisor_timer_interrupt) {
        clock_set_next_event();
        do_timer();
    }
}