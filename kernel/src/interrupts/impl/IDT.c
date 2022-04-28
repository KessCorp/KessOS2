#include <interrupts/IDT.h>

// 2022 Ian Moffett <ian@kesscoin.com>

static struct IDTVector idt[256];
static struct IDTR idtr;


void set_idt_vec(uint8_t vec, void* isr, int flags) {
	uint64_t addr = (uint64_t)isr;
	idt[vec].isr_low16 = (addr) & 0xFFFF;
	idt[vec].isr_mid16 = (addr >> 16) & 0xFFFF;
	idt[vec].isr_high32 = (addr >> 32) & 0xFFFFFFFF;
	idt[vec].attr = flags;
	idt[vec].reserved = 0x0;
	idt[vec].kernel_cs = 0x08;
	idt[vec].ist = 0x0;
}


void idt_install() {
	idtr.limit = sizeof(struct IDTVector) * 256 - 1;
	idtr.base = (uint64_t)&idt;
	__asm__ __volatile__("lidt %0" :: "m" (idtr));
}
