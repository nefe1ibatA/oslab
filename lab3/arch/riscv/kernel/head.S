.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # ------------------
    # - your code here -
    # ------------------

    # set stvec = _traps
    la t1, _traps
    csrw stvec, t1

    # set sie[STIE] = 1
    addi t1, x0, 32
    csrs sie, t1

    # set first time interrupt
    rdtime t1
    li t2, 10000000
    mv a7, x0
    mv a6, x0
    add a0, t1, t2
    mv a1, x0
    mv a2, x0
    mv a3, x0
    mv a4, x0
    mv a5, x0
    ecall
    la sp, boot_stack_top
    call mm_init
    call task_init
    # set sstatus[SIE] = 1
    csrsi sstatus, 2

    # ------------------

    
    jal x0, start_kernel

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: