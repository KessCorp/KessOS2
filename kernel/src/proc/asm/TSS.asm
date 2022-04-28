global load_tss
global _context_switch
extern userspace_entry
bits 64

;; 2022 Ian Moffett <ian@kesscoin.com>


load_tss:
    mov ax, 40
    push bx
    str bx
    pop bx
    ltr ax
    retq


_context_switch:
    mov rsp, context_switch_struct
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq

context_switch_struct:
    times 15 dq 0       ; Zeros out register space.
    dq userspace_entry
    dq 0x18 | 3         ; Bottom two bits for CS is CPL. (Current priv level).
    dq 0x0
    dq 0x10000          ; Usermode needs a new stack.
    dq 0x20 | 3         ; Requested priv level.


