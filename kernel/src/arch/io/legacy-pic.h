#ifndef LEGACY_PIC_H
#define LEGACY_PIC_H

#define PIC1_COMMAND 0x20       // Master chip.
#define PIC1_DATA 0x21          // Data line.
#define PIC2_COMMAND 0xA0       // Slave chip.
#define PIC2_DATA 0xA1          // Data line.

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01
#define PIC_EOI 0x20

void init_pic();
void PIC_sendEOI(unsigned char irq);

#endif
