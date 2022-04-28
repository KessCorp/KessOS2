bits 64
global userspace_entry


userspace_entry:
    mov rax, 0x2
    mov r14, 50
    mov r13, 50
    mov r12, 150
    mov r11, 150
    mov r10, 0xFFFFFF
    int 0x80
    jmp $
