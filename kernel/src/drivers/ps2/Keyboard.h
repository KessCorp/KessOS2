#ifndef PS2_KEYBOARD_H
#define PS2_KEYBOARD_H

#include <interrupts/IDT.h>
#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>


struct PS2Keystroke {
    uint32_t scancode;
    char ch;
    uint8_t pressed : 1;
};


void ps2_keyboard_init();
__attribute__((interrupt)) void irq1_handler(struct InterruptFrame*);

#endif
