#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <interrupts/IDT.h>

// 2022 Ian Moffett <ian@kesscoin.com>


void set_pit_phase(uint32_t hz);
void pit_sleep(uint64_t _ticks);
__attribute__((interrupt)) void irq0_handler(struct InterruptFrame*);

#endif
