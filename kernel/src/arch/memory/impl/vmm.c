#include <arch/memory/vmm.h>
#include <util/mem.h>

// 2022 Ian Moffett <ian@kesscoin.com>


__attribute__((aligned(4096))) struct MappingTable pml4;

static uint64_t placement = 0x000000000E646000;
memdesc_t biggestSegment;

static void set_placement(meminfo_t meminfo) {
    uint64_t mmap_entries = get_mmap_entries(meminfo);

    biggestSegment.nPages = 0;

    for (int i = 0; i < mmap_entries; ++i) {
        memdesc_t* cur = mmap_iterator_helper(i, meminfo);
        if (cur->nPages > biggestSegment.nPages && cur->type == MMAP_USABLE_MEMORY) {
            biggestSegment = *cur;
        }
    }

    placement = (uint64_t)biggestSegment.physAddr;
}


void* alloc_page() {
    if (biggestSegment.nPages == 0) return NULL;    // No pages left to allocate!

    void* page = (void*)placement;
    placement += 4096;
    --biggestSegment.nPages;
    return page;
}


void map_page(uint64_t logical) {
    int flags = PAGE_BIT_P_PRESENT | PAGE_BIT_RW_WRITABLE | PAGE_BIT_US_USER;

    int pml4_idx = (logical >> 39) & 0x1ff;
    int pdp_idx = (logical >> 30) & 0x1ff;
    int pd_idx = (logical >> 21) & 0x1ff;
    int pt_idx = (logical >> 12) & 0x1ff;
    int p_idx = logical & 0x7ff;

    if (!(pml4.entries[pml4_idx] & PAGE_BIT_P_PRESENT)) {
        uint64_t pdpt_alloc = (uint64_t)alloc_page();
        memzero((void*)(uint64_t)(pdpt_alloc & PAGE_ADDR_MASK), 4096);
        pml4.entries[pml4_idx] = (pdpt_alloc & PAGE_ADDR_MASK) | flags;
        map_page(pdpt_alloc);
    }

    struct MappingTable* pdpt = (struct MappingTable*)(pml4.entries[pml4_idx] & PAGE_ADDR_MASK);
    if (!(pdpt->entries[pdp_idx] & PAGE_BIT_P_PRESENT)) {
        uint64_t pdt_alloc = (uint64_t)alloc_page();
        memzero((void*)pdt_alloc, 4096);
        pdpt->entries[pdp_idx] = (pdt_alloc & PAGE_ADDR_MASK) | flags;
        map_page(pdt_alloc);
    }

    struct MappingTable* pdt = (struct MappingTable*)(pdpt->entries[pdp_idx] & PAGE_ADDR_MASK);
    if (!(pdt->entries[pd_idx] & PAGE_BIT_P_PRESENT)) {
        uint64_t pt_alloc = (uint64_t)alloc_page();
        memzero((void*)pt_alloc, 4096);
        pdt->entries[pd_idx] = (pt_alloc & PAGE_ADDR_MASK) | flags;
        map_page(pt_alloc);
    }

    struct MappingTable* pt = (struct MappingTable*)(pdt->entries[pd_idx] & PAGE_ADDR_MASK);
    if (!(pt->entries[pt_idx] & PAGE_BIT_P_PRESENT)) {
        pt->entries[pt_idx] = (logical & PAGE_ADDR_MASK) | flags;
    }
}


void init_vmm(meminfo_t meminfo) {
    set_placement(meminfo);
    memzero(&pml4, 4096);
    for (uint64_t i = 0; i < GB*6; i+=4096) {
        map_page(i);
    }

    load_pml4(&pml4);
}
