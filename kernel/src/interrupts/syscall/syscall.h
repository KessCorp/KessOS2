#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

void syscall_dispatcher(uint64_t rax, uint64_t r14, uint64_t r13, uint64_t r12, uint64_t r11, uint64_t r10);
void syscall_gate();

#endif
