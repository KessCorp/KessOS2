#include <drivers/ps2/Keyboard.h>
#include <arch/io/io.h>
#include <arch/io/legacy-pic.h>
#include <debug/log.h>

// 2022 Ian Moffett <ian@kesscoin.com>


struct PS2Keystroke ps2_keystroke_buffer;

static const uint8_t* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
"\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
"zxcvbnm,./" "\x00\x00\x00" " ";


void ps2_keyboard_init() {
    log("Initalizing ps/2 keyboard..\n", S_INFO);
    outportb(PIC1_DATA, inportb(PIC1_DATA) ^ (1 << 1));
}


__attribute__((interrupt)) void irq1_handler(struct InterruptFrame*) {   
    ps2_keystroke_buffer.scancode = inportb(0x60);
    ps2_keystroke_buffer.ch = SC_ASCII[ps2_keystroke_buffer.scancode];

    if (!(ps2_keystroke_buffer.scancode & 0x80)) {
        ps2_keystroke_buffer.pressed = 1;
    }

    PIC_sendEOI(0x1);
}
