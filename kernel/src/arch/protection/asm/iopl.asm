;; 2022 Ian Moffett <ian@kesscoin.com>

global iopl_zero


;; This will make sure IO operations can only operate in 
;; ring zero.
iopl_zero:
    pushf                   ;; Push flags onto stack.
    pop rax                 ;; Save it in RAX.
    and rax, ~(1 << 12)     ;; Clear bit 12.
    and rax, ~(1 << 13)     ;; Clear bit 13.

    ;; Now it should be zero and can only be accessed 
    ;; in ring zero.
    push rax                ;; Push RAX back onto stack.
    popf                    ;; Pop it into FLAGS.
    retq
