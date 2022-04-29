#ifndef PIC_H
#define PCI_H

#include <stdint.h>

// 2022 Ian Moffett <ian@kesscoin.com>

uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint8_t pci_valid_vendor(uint8_t bus, uint8_t slot);
uint16_t pci_read_vendor_id(uint8_t bus, uint8_t device, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t device, uint8_t func);
uint16_t pci_get_class_id(uint8_t bus, uint8_t device, uint8_t func);
uint16_t pci_get_subclass_id(uint8_t bus, uint8_t device, uint8_t func);
void pci_enumerate_and_log();

#endif
