#ifndef PIC_H
#define PCI_H

#include <stdint.h>

uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint8_t pic_valid_vendor(uint8_t bus, uint8_t slot);


#endif
