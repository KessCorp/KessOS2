#include <arch/io/legacy-pic.h>
#include <arch/io/io.h>


void init_pic() {
  outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);      // Init master PIC.
  io_wait();
  outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);      // Init slave PIC.
  io_wait();

  outportb(PIC1_DATA, 0x20);
  io_wait();
  outportb(PIC2_DATA, 0x28);
  io_wait();

  outportb(PIC1_DATA, 4);
  io_wait();
  outportb(PIC2_DATA, 2);
  io_wait();

  outportb(PIC1_DATA, ICW4_8086);
  io_wait();
  outportb(PIC2_DATA, ICW4_8086);
  io_wait();

  outportb(PIC1_DATA, 0xFF);
  io_wait();
  outportb(PIC2_DATA, 0xFF);
}


void PIC_sendEOI(unsigned char irq) {
  if(irq >= 8)
    outportb(PIC2_COMMAND,PIC_EOI);

  outportb(PIC1_COMMAND,PIC_EOI);
}
