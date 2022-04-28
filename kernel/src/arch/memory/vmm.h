#ifndef VMM_H
#define VMM_H

#include <arch/memory/memory.h>
#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>


#define PAGE_BIT_P_PRESENT (1<<0)
#define PAGE_BIT_RW_WRITABLE (1<<1)
#define PAGE_BIT_US_USER (1<<2)
#define PAGE_XD_NX (1<<63)

#define PAGE_ADDR_MASK 0x000ffffffffff000

#define PAGE_BIT_A_ACCESSED (1<<5)
#define PAGE_BIT_D_DIRTY (1<<6)
#define GB 0x40000000UL

struct MappingTable {
    uint64_t entries[512];
};


void load_pml4(struct MappingTable* pml4);
void* alloc_page();
void map_page(uint64_t logical);
void init_vmm(meminfo_t meminfo);

#endif
