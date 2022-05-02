#ifndef PCI_H
#define PCI_H

// 2022 Ian Moffett <ian@kesscoin.com>


#include <arch/io/io.h>
#include <stdint.h>


struct PCIDevice {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint8_t valid;
};


uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_classid(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_subclass_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_prog_if(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_revision_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_vendor_is_valid(uint16_t vendor_id);
void pci_enumerate_and_log();


#endif
