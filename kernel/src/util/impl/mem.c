#include <util/mem.h>


void memzero(void* dest, size_t n) {
    char* dst = (char*)dest;

    for (int i = 0; i < n; ++i) {
        dst[i] = 0x0;
    }
}


void memcpy(void* dest, void* src, size_t n) {
    char* dch = (char*)dest;
    char* sch = (char*)src;

    for (int i = 0; i < n; ++i) {
        dch[i] = sch[i];
    }
}
