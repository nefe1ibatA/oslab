.extern start_kernel

    .section .text.init
    .globl _start
_start:
    # ------------------
    # - Lab4 code here -
    # ------------------

    call setup_vm
    call relocate

    # ------------------
    # - Lab2 code here -
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

    la sp, boot_stack_top   # lab1 code

    call mm_init
    call task_init

    call setup_vm_final

    # set sstatus[SIE] = 1
    csrsi sstatus, 2

    # ------------------

    
    jal x0, start_kernel    # lab1 code


relocate:
    # ------------------
    # - Lab4 code here -
    # ------------------

    # ------------------

        # set ra = ra + PA2VA_OFFSET 0xffff ffe0 0000 0000
        #                           -0x0000 0000 8000 0000
        #                           =0xffff ffdf 8000 0000
        # set sp = sp + PA2VA_OFFSET (If you have set the sp before)
        li t0, 0xffffffdf80000000
        add ra, ra, t0
        add sp, sp, t0
        
    # ------------------

    # ------------------

        # set satp with early_pgtbl
        li t0, 8 << 60
        la t1, early_pgtbl
        srli t1, t1, 12
        add t0, t0, t1
        csrw satp, t0

    # ------------------

    # flush tlb
    sfence.vma zero, zero

    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 4096 # <-- change to your stack size

    .globl boot_stack_top
boot_stack_top: