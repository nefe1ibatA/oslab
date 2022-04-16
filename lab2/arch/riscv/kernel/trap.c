#include "printk.h"

#define supervisor_timer_interrupt 0x8000000000000005

extern void clock_set_next_event();

void trap_handler(unsigned long scause, unsigned long sepc) {
    if (scause == supervisor_timer_interrupt) {
        printk("[S] Supervisor Mode Timer Interrupt\n");
        clock_set_next_event();
    }
}