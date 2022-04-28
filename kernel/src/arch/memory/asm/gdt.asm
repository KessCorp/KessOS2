global load_gdt
global GDT_TSS

;; 2022 Queso Fuego, Ian Moffett <ian@kesscoin.com>



GDT64:                               ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64             ; The null descriptor.
    ; dw 0xFFFF                      ; Limit (low).
    ; dw 0                           ; Base (low).
    ; db 0                           ; Base (middle)
    ; db 0                           ; Access.
    ; db 1                           ; Granularity.
    ; db 0                           ; Base (high).
    dd 0x0
    dd 0x0
    .Code: equ $ - GDT64             ; The code descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10011010b                 ; Access (exec/read).
        db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .Data: equ $ - GDT64             ; The data descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 10010010b                 ; Access (read/write).
        db 00000000b                 ; Granularity.
        db 0
    .UserCode: equ $ - GDT64         ; The code descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11111010b                 ; Access (exec/read).
        db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
        db 0                         ; Base (high).
    .UserData: equ $ - GDT64         ; The data descriptor.
        dw 0                         ; Limit (low).
        dw 0                         ; Base (low).
        db 0                         ; Base (middle)
        db 11110010b                 ; Access (read/write).
        db 00000000b                 ; Granularity.
        db 0
    TSS:
        dq 0
        dq 0
    Pointer:                         ; The GDT-pointer.
        dw $ - GDT64 - 1             ; Limit.
        dq GDT64                     ; Base.


GDT_TSS: dq 0

load_gdt:
    cli
    mov rax, TSS
    mov [GDT_TSS], rax
    lgdt [Pointer]
    mov rax, 0x10
    mov ds, rax
    mov es, rax
    mov fs, rax
    mov gs, rax
    mov ss, rax
    mov rax, 0x8
    pop rdi
    push rax
    push rdi
    retfq
