#include <drivers/timer/PIT.h>
#include <util/asm.h>
#include <util/etc.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <debug/log.h>

// 2022 Ian Moffett <ian@kesscoin.com>

static uint64_t ticks = 0;


void set_pit_phase(uint32_t hz) {
    uint16_t divisor = 1193180 / hz;
    outportb(0x43, 0x36);        		// Command byte.
    outportb(0x40, divisor & 0xFF); // Low 8 bytes of divisor.
    outportb(0x40, divisor >> 8);   // High 8 bytes of divisor.
}



void pit_sleep(uint64_t _ticks) {
    uint64_t eticks = ticks + _ticks;
    UNGLITCH_ARG(_ticks);
    while (ticks < eticks);
}


__attribute__((interrupt)) void irq0_handler(struct InterruptFrame*) {
    ++ticks;
    PIC_sendEOI(0x0);
}
