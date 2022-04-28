#include <proc/TSS.h>
#include <arch/memory/vmm.h>

// 2022 Ian Moffett <ian@kesscoin.com>

extern tss_desc* GDT_TSS;

void load_tss();

tss_entry_t tss_entry = {0};

void init_tss() {
    uint64_t stack = (uint64_t)alloc_page();
    stack += 4080;      // So it doens't overwrite page.
    tss_entry.rsp0Low = stack & 0xFFFFFFFF;
    tss_entry.rsp0High = (stack & 0xFFFFFFFF00000000) >> 32;

    uint64_t tssp = (uint64_t)&tss_entry; 

    GDT_TSS->baseAddrLow = tssp & 0xFFFF;
    GDT_TSS->baseAddrMiddle = (tssp & 0xFF0000) >> 16;
    GDT_TSS->baseAddrOtherMiddle = (tssp & 0xFF000000) >> 24;
    GDT_TSS->baseAddrUpper = (tssp & 0xFFFFFFFF00000000) >> 32;
    GDT_TSS->p = 0x1;
    GDT_TSS->type = 0x9;
    GDT_TSS->zero = 0x00;
    GDT_TSS->dpl = 0;
    GDT_TSS->seglimit = sizeof(tss_entry);
    GDT_TSS->g = 0x1;
    load_tss();
    return;
}
