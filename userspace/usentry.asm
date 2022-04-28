bits 64
global userspace_entry


userspace_entry:
    mov rax, 0x2
    mov r14, 0
    mov r13, 0
    mov r12, 500
    mov r11, 30
    mov r10, 0xFFFFFF
    int 0x80
    
    mov rax, 0x1
    int 0x80

    jmp $
