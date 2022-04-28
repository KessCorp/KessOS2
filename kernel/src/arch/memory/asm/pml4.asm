bits 64
global load_pml4

;; 2022 Ian Moffett <ian@kesscoin.com>


load_pml4:
    and rdi, rax
    mov cr3, rdi
    retq
