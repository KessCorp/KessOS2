#ifndef PCI_H
#define PCI_H

// 2022 Ian Moffett <ian@kesscoin.com>


#include <arch/io/io.h>
#include <stdint.h>

#define PCI_HIGHEST_BUS 255
#define PCI_HIGHEST_SLOT 32
#define PCI_HIGHEST_FUNC 8

/*
 *  You can loop through everything with
 *  a nested for loop using all these
 *  macros above as max values.
 */


struct PCIDevice {
    uint8_t bus;
    uint8_t slot;
    uint8_t func;
    uint8_t valid;
};


struct BarSpace {
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
};


typedef enum {
    NOT_FINALIZED,
    MASS_STORAGE_CONTROLLER,
    NETWORK_CONTROLLER,
    DISPLAY_CONTROLLER,
    MULTI_MEDIA_DEVICE,
    MEMORY_CONTROLLER,
    BRIDGE_DEVICE,
    SCC,
    BASE_SYSTEM_PERIPHERALS,
    INPUT_DEVICES,
    DOCKING_STATIONS,
    PROCESSORS,
    SERIAL_BUS_CONTROLLERS,
    WIRELESS_CONTROLLER,
    INTELLIGENT_IO_CONTROLLER,
    SATELLITE_COMMUNICATION_CONTROLLERS,
    DASP_CONTROLLER,
    PROCESSING_ACCESLERATORS,
    NE,
    RESERVED
} BASE_CLASS;


uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_classid(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_subclass_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_prog_if(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_get_revision_id(uint8_t bus, uint8_t slot, uint8_t func);
uint8_t pci_vendor_is_valid(uint16_t vendor_id);
uint32_t pci_get_bar0(uint8_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar1(uint8_t bus, uint8_t slot, uint8_t func);
void pci_enumerate_and_log();


#endif
