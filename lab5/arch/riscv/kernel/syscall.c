#include "syscall.h"
#include "proc.h"
#include "printk.h"

extern struct task_struct *current;

void syscall(pt_regs *regs)
{
    if (regs->a[7] == SYS_WRITE) {
        if (regs->a[0] == 1) {
            char *str = (char *)regs->a[1];
            uint64 len = regs->a[2];
            str[len] = '\0';
            regs->a[0] = printk(str);
        }
    }
    else if (regs->a[7] == SYS_GETPID) {
        regs->a[0] = current->pid;
    }
    regs->sepc += 0x4;
}