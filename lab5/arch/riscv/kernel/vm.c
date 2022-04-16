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
    int VPNs[3];
    // 需要映射的段 的末Byte位置
    uint64 addr_last_B = va + sz - 1;

    uint64 *pte_p; // 指向页表项的指针

    // 为需要映射的段 逐页建立映射
    for (int cnt = 1; va <= addr_last_B; va += PGSIZE, pa += PGSIZE)
    {
        uint64 vpns = (va >> 12) & 0x7ffffff;

        VPNs[0] = vpns >> 0 & 0x1ff;
        VPNs[1] = vpns >> 9 & 0x1ff;
        VPNs[2] = vpns >> 18 & 0x1ff;
        uint64 *pt_p = pgtbl; // 循环中当前页表的起始地址

        // 分层进行，从根页表开始
        for (int VPNidx = 2; VPNidx > 0; VPNidx--)
        {
            pte_p = &pt_p[VPNs[VPNidx]]; // pt_p所指的页表中 VPNs[VPNidx]那项 即为目的项
            // 若该页表项有效 获得其指明的下一级页表
            if (*pte_p & 0x01)
            {
                // pte_PPNS <= 该页表项的[53:10] 即PPN[2]~PPN[0]
                uint64 pte_PPNs = (uint64)((uint64)(*pte_p) >> 10) & (uint64)0xfffffffffff; //
                // 页表项的PPNs -> PA
                // pt_p = (uint64*)((pte_PPNs << 12) | 0xff00000000000000);
                pt_p = (uint64 *)(pte_PPNs << 12);
                pt_p = (uint64 *)((uint64)pt_p + PA2VA_OFFSET);
            }
            // 若该页表项无效 为其分配一页
            else
            {
                pt_p = (uint64 *)kalloc();
                // 让pte_p指向的页表项 指向分配新页
                uint64 PPN = (((uint64)pt_p - PA2VA_OFFSET) >> 12) & 0xfffffffffff; // debug发现应保留高8位 因此52位全取
                *pte_p = (uint64)(PPN << 10) |                                      // 设置页表项的PPN[2]~PPN[0]
                         (uint64)0x1;                                               // 设置页表项的protection bits
            }
        }
        // VPNidx = 0, pt_p所指为第三级页表项
        pte_p = &pt_p[VPNs[0]];
        uint64 PPN = (pa >> 12) & (0xfffffffffff); // debug发现应保留高8位 因此52位全取
        *pte_p = (PPN << 10) |                     // 设置页表项的PPN[2]~PPN[0]
                 ((uint64)perm);                   // 设置页表项的protection bits
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
    printk("setup_vm_final done!\n");

    // flush TLB 
    asm volatile("sfence.vma zero, zero"); 
    return;
}

