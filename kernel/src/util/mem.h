#ifndef MEM_H
#define MEM_H

#include <stddef.h>

void memzero(void* dest, size_t n);
void memcpy(void* dest, void* src, size_t n);

#endif
