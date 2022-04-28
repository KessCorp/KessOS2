#include <arch/io/io.h>

// 2022 Ian Moffett <ian@kesscoin.com>

unsigned char inportb(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
 }


void outportb(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

void outportw(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

unsigned short inportw(unsigned short port) {
    unsigned short data;
    __asm__ __volatile__("inw %w1, %w0" : "=a" (data) : "Nd" (port));
}


void outportl(unsigned short port, uint32_t data) {
    __asm__ __volatile__("outl %0, %w1" : : "a" (data), "Nd" (port));
}


uint32_t inportl(unsigned short port) {
    uint32_t data;
    __asm__ __volatile__("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}


void io_wait() {
    __asm__("out %%al, %%dx" : :"a" (0x0), "d" (0x80));
}
