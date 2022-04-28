bits 64
global userspace_entry


userspace_entry:
    mov rax, 0x1
    int 0x80
    jmp $
