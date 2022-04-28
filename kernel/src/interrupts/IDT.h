#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define IDT_INT_GATE_USER 0xEE


struct __attribute__((packed)) IDTVector {
	uint16_t isr_low16;
	uint16_t kernel_cs;
	uint8_t ist;
	uint8_t attr;
	uint16_t isr_mid16;
	uint32_t isr_high32;
	uint32_t reserved;
};



struct __attribute__((packed)) IDTR {
	uint16_t limit;
	uint64_t base;
};



struct __attribute__((packed)) InterruptFrame {
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
};


void set_idt_vec(uint8_t vec, void* isr, int flags);
void idt_install();


#endif
