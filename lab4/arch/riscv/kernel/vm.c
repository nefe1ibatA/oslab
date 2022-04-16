#include "defs.h"
#include "string.h"
#include "mm.h"
#include "printk.h"

extern char _stext[];
extern char _srodata[];
extern char _sdata[];

unsigned long early_pgtbl[512] __attribute__((__aligned__(0x1000)));

void setup_vm(void) 
{
    early_pgtbl[2] = early_pgtbl[384] = (2 << 28) + 15;
}

unsigned long swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm)
{
    /* 
    pgtbl 为根⻚表的基地址 
    va, pa 为需要映射的虚拟地址、物理地址 
    sz 为映射的⼤⼩ 
    perm 为映射的读写权限，可设置不同section所在⻚的属性，完成对不同section的保护

    创建多级⻚表的时候可以使⽤ kalloc() 来获取⼀⻚作为⻚表⽬录 
    可以使⽤ V bit 来判断⻚表项是否存在 
    */

    uint64 vpn[3];
    for (; sz > 0; sz -= PGSIZE, pa += PGSIZE, va += PGSIZE) {
        vpn[2] = (va << 25) >> 55;
        vpn[1] = (va << 34) >> 55;
        vpn[0] = (va << 43) >> 55;

        uint64 *pte;
        uint64 *cur = pgtbl;
        for (int i = 2; i > 0; i--) {
            pte = &cur[vpn[i]];
            if (*pte & 1) {
                cur = (uint64*)(((*pte << 10) >> 20) << 12);
            }
            else {
                uint64 *next = (uint64*)(kalloc() - PA2VA_OFFSET);
                *pte = (((uint64)next >> 12) << 10) | 1;
                cur = next;
            }
        }
        cur[vpn[0]] = ((pa << 10) >> 12) | perm;
    }
}

void setup_vm_final(void)
{
    memset(swapper_pg_dir, 0x0, PGSIZE);

    // No OpenSBI mapping required

    // mapping kernel text X|-|R|V 
    create_mapping(swapper_pg_dir,
                   (uint64)_stext, 
                   (uint64)_stext - PA2VA_OFFSET,
                   (uint64)_srodata - (uint64)_stext,
                   11);

    // mapping kernel rodata -|-|R|V 
    create_mapping(swapper_pg_dir,
                   (uint64)_srodata,
                   (uint64)_srodata - PA2VA_OFFSET,
                   (uint64)_sdata - (uint64)_srodata,
                   3);

    // mapping other memory -|W|R|V 
    create_mapping(swapper_pg_dir,
                   (uint64)_sdata,
                   (uint64)_sdata - PA2VA_OFFSET,
                   (uint64)PHY_END - (uint64)_sdata + PA2VA_OFFSET,
                   7);

    // set satp with swapper_pg_dir
    uint64 _satp = (uint64)1 << 63;
    _satp += ((uint64)swapper_pg_dir - PA2VA_OFFSET) >> 12;
    csr_write(satp, _satp);

    // YOUR CODE HERE

    // flush TLB 
    asm volatile("sfence.vma zero, zero"); 

    // uint64 *p = 0xffffffe000202000; 
    // printk("%lx\n", *p);

    // uint64 raw = 0x12345;
    // *p = raw;
    // printk("%lx\n", *p);
    return;
}