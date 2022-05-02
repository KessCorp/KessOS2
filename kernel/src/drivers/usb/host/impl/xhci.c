#include <drivers/usb/host/xhci.h>
#include <drivers/pci/pci.h>

// 2022 Ian Moffett <ian@kesscoin.com>


static struct PCIDevice xhci_controller;
static uint64_t mmio_addr;


uint8_t xhci_init() {
    for (int bus = 0; bus < PCI_HIGHEST_BUS; ++bus) {
        for (int slot = 0; slot < PCI_HIGHEST_SLOT; ++slot) {
            for (int func = 0; func < PCI_HIGHEST_FUNC; ++func) {
                uint8_t classid = pci_get_classid(bus, slot, func);
                uint8_t subclass_id = pci_get_subclass_id(bus, slot, func);
                uint8_t interface_byte = pci_get_prog_if(bus, slot, func);

                if (classid == SERIAL_BUS_CONTROLLERS && subclass_id == 0x03 && interface_byte == 0x30) {
                    struct PCIDevice xhci = {
                        .bus = bus,
                        .slot = slot,
                        .func = func,
                        .valid = 1
                    };

                    xhci_controller = xhci;

                    // Get MMIO address by combining BAR0 and BAR1. (I guess you just OR them together).
                    mmio_addr = pci_get_bar0(bus, slot, func) | pci_get_bar1(bus, slot, func);
                    return 1;
                }
            }
        }
    }


    struct PCIDevice no_xhci = {
        .valid = 0
    };

    return 0;   // No XHCI controller found.

}
