#include <drivers/video/FrameBuffer.h>
#include <drivers/timer/PIT.h>
#include <drivers/audio/pcspkr.h>
#include <drivers/pci/pci.h>
#include <debug/log.h>
#include <arch/memory/memory.h>
#include <arch/memory/gdt.h>
#include <arch/memory/vmm.h>
#include <arch/io/legacy-pic.h>
#include <arch/io/io.h>
#include <interrupts/IDT.h>
#include <interrupts/exceptions.h>
#include <interrupts/syscall/syscall.h>
#include <util/asm.h>
#include <proc/TSS.h>


canvas_t canvas = {
    .x = 0,
    .y = 55,
    .prevX = 0
};


uint8_t gLegacyModeEnabled = 0;

void log(const char* format, STATUS status, ...) {
  va_list ptr;

  size_t args = 0;

  for (int i = 0; i < strlen(format); ++i) {
    if (format[i] == '%') {
      switch (format[i + 1]) {
      case 's':
      case 'd':
        ++args;
        i += 2;
        break;
      }
    }
  }

  // A hacky way to get rid of the warning: second parameter of 'va_start' not last named argument
  STATUS s = status;
  status ^= status;
  status += args;
  va_start(ptr, status);

  int color = 0xFFFFFF;

  switch (s) {
  case S_WARNING:
    color = 0x800080;
    kwrite(&canvas, "[WARNING] ", color);
    break;
  case S_INFO:
    color = 0x00FF00;
    kwrite(&canvas, "[INFO] ", color);
    break;
  case S_PANIC:
    color = 0xFF0000;
    kwrite(&canvas, "** KERNEL PANIC **\n", color);
    break;
  case S_CRITICAL:
    color = 0xFF0000;
    kwrite(&canvas, "[CRITICAL] ", color);
    break;
  }

  for (int i = 0; i < strlen(format); ++i) {
    if (format[i] == '%') {
      switch (format[i + 1]) {
      case 's':
        {
          const char* arg = va_arg(ptr, const char*);

          for (int j = 0; j < strlen(arg); ++j) {
            char terminated[2] = {arg[j], 0x0};
            kwrite(&canvas, terminated, color);
          }

          ++i;

        }

        continue;
      case 'd':
        {
          int arg = va_arg(ptr, int);

          kwrite(&canvas, dec2str(arg), color);
        }
        ++i;
        continue;
      case 'x':
        {
          int arg = va_arg(ptr, int);
          kwrite(&canvas, hex2str(arg), color);
          ++i;
          continue;
        }
      }
    }

    char terminated[2] = {format[i], 0x0};
    kwrite(&canvas, terminated, color);
  }
}


static void init(meminfo_t meminfo) {
    load_gdt();

    // Init TSS.
    init_tss();

    // Exceptions.
    log("Setting up exceptions..\n", S_INFO);
    set_idt_vec(0x0, div0_handler, TRAP_GATE_FLAGS);
    set_idt_vec(0x1, debug_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0x4, overflow_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0x5, bre_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0x6, invalid_opcode_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0x7, dev_not_avail_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0x8, double_fault, TRAP_GATE_FLAGS);
    set_idt_vec(0xA, invalid_tss_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0xB, snp_exception, TRAP_GATE_FLAGS);
    set_idt_vec(0xC, stackseg_fault, TRAP_GATE_FLAGS);
    set_idt_vec(0xD, gpf, TRAP_GATE_FLAGS);
    set_idt_vec(0xE, page_fault, TRAP_GATE_FLAGS);
    
    log("Installing Interrupt Descriptor Table..\n", S_INFO);
    idt_install();

    // Setup PIC.
    log("Setting up legacy PIC (legacy mode is off, will use APIC one day\n", S_INFO);
    init_pic();

    log("Disabling verbose mode. (Don't want to fill up screen and too lazy to implement scrolling).\n", S_INFO);
    // Setup un-exceptions.
    set_idt_vec(0x20, irq0_handler, INT_GATE_FLAGS);
    set_idt_vec(0x80, syscall_gate, IDT_INT_GATE_USER);

    // Init PIT.
    set_pit_phase(100);

    // Unmask required IRQ channels.
    outportb(PIC1_DATA, inportb(PIC1_DATA) ^ 1);

    // Init VMM.
    init_vmm(meminfo);

}


int _start(framebuffer_t* lfb, psf1_font_t* font, meminfo_t meminfo, void* rsdp, uint8_t legacy_mode) {
    canvas.font = font;
    canvas.lfb = lfb;
    gLegacyModeEnabled = legacy_mode;

    CLI;
    init(meminfo);
    STI;

    pit_sleep(150);

    pci_enumerate_and_log();
    pit_sleep(150);

    CLI;
    clearScreen(&canvas, 0x000000);
    STI;

    log("Hello, World!\n", S_INFO);

    pcspkr_play(500);
    pit_sleep(25);
    pcspkr_shutup();

    CLI;
    // Enter userspace.
    _context_switch();
    
    while (1) {
        HLT;
    }
}
