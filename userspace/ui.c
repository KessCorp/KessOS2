void ui_start() {
    __asm__ __volatile__("mov $0x1, %rax; int $0x80");

    // Draw a little bar.
    __asm__ __volatile__(
                        "mov $0x2, %rax\n"
                        "mov $0, %r14\n"
                        "mov $0, %r13\n"
                        "mov $500, %r12\n"
                        "mov $30, %r11\n"
                        "mov $0xFFFFFF, %r10\n"
                        "int $0x80"
                        );
    while (1);
}
