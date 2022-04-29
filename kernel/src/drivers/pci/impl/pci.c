#include <drivers/pci/pci.h>
#include <drivers/video/FrameBuffer.h>
#include <debug/log.h>
#include <arch/io/io.h>
#include <util/asm.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC


uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t addr;
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    // Create configuration address.
    addr = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // We wanna read from here.
    outportl(CONFIG_ADDRESS, addr);
    tmp = (uint16_t)((inportl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}



uint8_t pci_valid_vendor(uint8_t bus, uint8_t slot) {
    // If vendor ID is 0xFFFF that means it is non-existant.
    if (pci_read_word(bus, slot, 0, 0) == 0xFFFF) return 0;
    return 1;
}


// Used for counting devices.
void pci_enumerate_and_log() {
    uint64_t count = 0;
    uint64_t tmp = 0;

    for (int bus = 0; bus < 256; ++bus) {
        for (int slot = 0; slot < 32; ++slot) {
            if (pci_valid_vendor(bus, slot)) {
                log("Device found on bus %d and slot %d.\n", S_INFO, bus, slot);
                ++count;
                ++tmp;
            }

            if (tmp == 50) {
                extern canvas_t canvas;
                tmp = 0;
                CLI;
                clearScreen(&canvas, 0x000000);
                STI;
            }
        }
    }

    log("%d devices found!\n", S_INFO, count);
}
