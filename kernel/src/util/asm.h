#ifndef ASM_H
#define ASM_H


#define HALT __asm__ __volatile__("cli; hlt")
#define HLT __asm__ __volatile__("hlt")
#define CLI __asm__ __volatile__("cli")
#define STI __asm__ __volatile__("sti")
#define SECTION_DATA __attribute__((section(".data")))

#endif
