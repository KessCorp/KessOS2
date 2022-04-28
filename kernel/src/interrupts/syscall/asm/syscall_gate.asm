bits 64
global syscall_gate

;; 2022 Ian Moffett <ian@kesscoin.com>

extern syscall_dispatcher

syscall_gate:
    mov rdi, rax
    mov rsi, r14
    mov rdx, r13
    mov rcx, r12
    mov r8, r11
    mov r9, r10
    call syscall_dispatcher
    iretq 
