#include "printk.h"
#include "defs.h"
#include "syscall.h"

#define supervisor_timer_interrupt 0x8000000000000005
#define page_fault 0xd
#define user_syscall 0x8

extern void clock_set_next_event();
extern void do_timer();

void trap_handler(unsigned long scause, unsigned long sepc, pt_regs *regs) {
    if (scause == supervisor_timer_interrupt) {
        clock_set_next_event();
        do_timer();
    }
    else if (scause == page_fault && sepc < (uint64)VM_START) {
        printk("vm init\n");
        sepc += PA2VA_OFFSET;
        csr_write(sepc, sepc);
        asm volatile("sret");
    }
    else if (scause == user_syscall) {
        syscall(regs);
    }
}