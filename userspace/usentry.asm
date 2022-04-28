bits 64
global userspace_entry

extern ui_start


userspace_entry:
    ; mov rax, 0x2
    ; mov r14, 0
    ; mov r13, 0
    ; mov r12, 500
    ; mov r11, 30
    ; mov r10, 0xFFFFFF
    ; int 0x80 

    jmp ui_start
