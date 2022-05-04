#include <drivers/ps2/Keyboard.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <debug/log.h>
#include <util/asm.h>

// 2022 Ian Moffett <ian@kesscoin.com>


static struct PS2Keystroke ps2_keystroke_buffer;

static const uint8_t* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
"\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
"zxcvbnm,./" "\x00\x00\x00" " ";


void ps2_keyboard_init() {
    log("Initalizing ps/2 keyboard..\n", S_INFO);
    outportb(PIC1_DATA, inportb(PIC1_DATA) ^ (1 << 1));         // Unmask channel for IRQ 1.
}


struct PS2Keystroke fetch_last_ps2_keystroke() {
    // If a program in userspace somehow calls this directly, it will GPF due to the instructions behind this macro.
    USER_INDIRECTION;
    return ps2_keystroke_buffer;
}


__attribute__((interrupt)) void irq1_handler(struct InterruptFrame*) {   
    ps2_keystroke_buffer.scancode = inportb(0x60);
    ps2_keystroke_buffer.ch = SC_ASCII[ps2_keystroke_buffer.scancode];

    if (!(ps2_keystroke_buffer.scancode & 0x80)) {
        ps2_keystroke_buffer.pressed = 1;
    }

    PIC_sendEOI(0x1);
}
