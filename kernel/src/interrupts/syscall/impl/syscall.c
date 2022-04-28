#include <interrupts/syscall/syscall.h>
#include <arch/io/io.h>
#include <drivers/video/FrameBuffer.h>
#include <debug/log.h>

#define SYSCALL_COUNT 3

static struct SyscallRegArgs {
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
} regs;


static void sys_reboot() {
    outportb(0x64, 0xFE);
}


static void sys_hello() {
    log("Hello syscall!\n", S_INFO);
}


static void sys_draw_square() {
    /*              
     *  X:      r14    OOOOOOOOOOO      Lit design btw.
     *  Y:      r13         OO
     *  W:      r12         OO
     *  H:      r11         OO
     *  Color:  r10         OO
     */

    extern canvas_t canvas;
    draw_square(canvas, (unsigned int)regs.r14, (unsigned int)regs.r13, (unsigned int)regs.r12, 
            (unsigned int)regs.r11, (unsigned int)regs.r10);
}


static void* syscall_table[] = {
    sys_reboot,
    sys_hello,
    sys_draw_square
};


void syscall_dispatcher(uint64_t rax, uint64_t r14, uint64_t r13, uint64_t r12, uint64_t r11, uint64_t r10) {
    if (rax >= SYSCALL_COUNT || rax < 0) return;
    void(*syscall)(void) = syscall_table[rax];
    
    // Prepare args.
    regs.r14 = r14;
    regs.r13 = r13;
    regs.r12 = r12;
    regs.r11 = r11;
    regs.r10 = r10;

    syscall();
}
