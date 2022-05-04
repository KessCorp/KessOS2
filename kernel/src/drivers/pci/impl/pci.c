#include <drivers/pci/pci.h>
#include <drivers/video/FrameBuffer.h>
#include <arch/io/io.h>
#include <debug/log.h>
#include <stddef.h>
#include <util/asm.h>

// 2022 Ian Moffett <ian@kesscoin.com>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

SECTION_DATA static const char* const BASE_CLASS_TYPES[] = {
    "NOT_FINALIZED",
    "MASS_STORAGE_CONTROLLER",
    "NETWORK_CONTROLLER",
    "DISPLAY_CONTROLLER",
    "MULTI_MEDIA_DEVICE",
    "MEMORY_CONTROLLER",
    "BRIDGE_DEVICE",
    "SCC",                              // Simple communication controllers.
    "BASE_SYSTEM_PERIPHERALS",
    "INPUT_DEVICES",
    "DOCKING_STATIONS",
    "PROCESSORS",
    "SERIAL_BUS_CONTROLLERS",
    "WIRELESS_CONTROLLER",
    "INTELLIGENT_IO_CONTROLLER",
    "SATELLITE_COMMUNICATION_CONTROLLERS",
    "ENCRYPTION/DECRYPTION CONTROLLERS",
    "DASP_CONTROLLER",
    "PROCESSING_ACCELERATORS",
    "NE",
    "RESERVED",
    "INVALID"
};


uint8_t vendor_is_valid(uint16_t vendor_id) {
    return vendor_id != 0xFFFF;
}


uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
 
    // Create configuration address.
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000)); 

    outportl(CONFIG_ADDRESS, address);
    tmp = (uint16_t)((inportl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}



void pci_enumerate_and_log() {
    size_t device_count = 0;
    size_t tmp = 0;

    for (int bus = 0; bus < 255; ++bus) {
        for (int slot = 0; slot < 32; ++slot) {
            for (int func = 0; func < 8; ++func) {
                if (tmp == 20) {
                    extern canvas_t canvas;
                    tmp = 0;
                    clearScreen(&canvas, 0x000000);
                }

                if (vendor_is_valid(pci_read_word(bus, slot, func, 0))) { 
                    const char* baseclass_type;

                    if (pci_get_classid(bus, slot, func) > 0x13) 
                        baseclass_type = "RESERVED";
                    else
                        baseclass_type = BASE_CLASS_TYPES[pci_get_classid(bus, slot, func)];

                    log("Device found with bus %d, slot %d, device ID %x, type: %s\n", S_INFO, bus, slot, pci_get_device_id(bus, slot, func), baseclass_type);
                    ++device_count;
                    ++tmp;
                }
            }
        }
    }

    log("%d devices found!\n", S_INFO, device_count);
}


uint8_t pci_vendor_is_valid(uint16_t vendor_id) {
    return vendor_id != 0xFFFF;
}


uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x0);
}


uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x2);
}


uint8_t pci_get_classid(uint8_t bus, uint8_t slot, uint8_t func) {
    return (pci_read_word(bus, slot, func, 0xA) & 0xFF00) >> 8;
}


uint8_t pci_get_subclass_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0xA) & 0x00FF;
}


uint8_t pci_get_prog_if(uint8_t bus, uint8_t slot, uint8_t func) {
    return (pci_read_word(bus, slot, func, 0x8) & 0xFF00) >> 8;
}


uint8_t pci_get_revision_id(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x8) & 0x00FF;
}


static uint16_t get_bar0_low(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x10);
}


static uint16_t get_bar0_high(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x12);    
}


static uint16_t get_bar1_low(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x14);
}


static uint16_t get_bar1_high(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_read_word(bus, slot, func, 0x16);
}


uint32_t pci_get_bar0(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t low = get_bar0_low(bus, slot, func);
    
    // Is it IO space BAR?
    if (low & 1) {
        low ^= (1 << 0);                                // Make bit 0 zero.
        if (low & (1 << 1)) low ^= (1 << 1);            // Make bit 1 zero if it is not zero.
    } else {
        if (low & (1 << 1)) low ^= (1 << 1);            // Make bit 1 zero if it is not zero.
        if (low & (1 << 2)) low ^= (1 << 2);            // Make bit 2 zero if it is not zero.
        if (low & (1 << 3)) low ^= (1 << 3);            // Make bit 3 zero if not zero.
    }

    uint32_t combined = (uint64_t)get_bar0_high(bus, slot, func) << 16 | low;
    return combined;
}


uint32_t pci_get_bar1(uint8_t bus, uint8_t slot, uint8_t func) {
    uint16_t low = get_bar1_low(bus, slot, func); 

    // Is it IO space BAR?
    if (low & 1) {
        low ^= (1 << 0);                                // Make bit 0 zero.
        if (low & (1 << 1)) low ^= (1 << 1);            // Make bit 1 zero if it is not zero.
    } else {
        if (low & (1 << 1)) low ^= (1 << 1);            // Make bit 1 zero if it is not zero.
        if (low & (1 << 2)) low ^= (1 << 2);            // Make bit 2 zero if it is not zero.
        if (low & (1 << 3)) low ^= (1 << 3);            // Make bit 3 zero if not zero.
    }

    uint32_t combined = (uint64_t)get_bar1_high(bus, slot, func) << 16 | low;
    return combined;
}
