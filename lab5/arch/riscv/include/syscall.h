#pragma once

#define SYS_WRITE 64
#define SYS_GETPID 172

#include "defs.h"
#include "types.h"

typedef struct pt_regs {
    uint64 a[22];
    uint64 sepc;
    uint64 sstatus;
} pt_regs;

void syscall(pt_regs *regs);