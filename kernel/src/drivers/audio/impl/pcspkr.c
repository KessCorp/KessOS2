// 2022 Ian Moffett <ian@kesscoin.com>

#include <drivers/audio/pcspkr.h>
#include <arch/io/io.h>
#include <util/asm.h>

void pcspkr_play(uint32_t hz) {
    CLI;
    UNGLITCH_ARG(hz);

    uint32_t div = 1193180 / hz;
    uint8_t tmp;

    outportb(0x43, 0xB6);   // Command byte.
    outportb(0x42, (uint8_t)(div & 0xFF));
    outportb(0x42, (uint8_t)(div >> 8));

    tmp = inportb(0x61);

    if (tmp != (tmp | 3))
        outportb(0x61, tmp | 3);

    STI;
}


void pcspkr_shutup() {
    uint8_t tmp = inportb(0x61) & 0xFC;
    outportb(0x61, tmp);
}

