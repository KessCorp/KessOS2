void ui_start() {
    __asm__ __volatile__("mov $0x1, %rax; int $0x80");
    while (1);
}
