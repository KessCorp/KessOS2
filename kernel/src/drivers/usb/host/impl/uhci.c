#include <drivers/usb/host/uhci.h>
#include <drivers/pci/pci.h>

// 2022 Ian Moffett <ian@kesscoin.com>


/*
 *  NOTE to reader:
 *
 *  If you have anything that supports
 *  UHCI let me know through my discord:
 *  Серая#0690
 *
 *
 *  I will only continue UHCI once I have
 *  testing resources for it.
 *
 */

static struct PCIDevice usb_controller;


uint8_t uhci_init() {
    for (int bus = 0; bus < PCI_HIGHEST_BUS; ++bus) {
        for (int slot = 0; slot < PCI_HIGHEST_SLOT; ++slot) {
            for (int func = 0; func < PCI_HIGHEST_FUNC; ++func) {
                uint8_t classid = pci_get_classid(bus, slot, func);
                uint8_t subclassid = pci_get_subclass_id(bus, slot, func);
                uint8_t interface_byte = pci_get_prog_if(bus, slot, func);

                // Check the if we found the correct classid/subclass id.
                if (classid == SERIAL_BUS_CONTROLLERS && subclassid == 0x03 && interface_byte == 0x00) {
                    struct PCIDevice usb = {
                        .bus = bus,
                        .slot = slot,
                        .func = func,
                        .valid = 1
                    };

                    usb_controller = usb;
                    return 1;                   // Yay! We found a UHCI controller!
                }
            }
        }
    }

    struct PCIDevice invalid_usb = {
        .valid = 0
    };

    usb_controller = invalid_usb;

    return 0;                   // No UHCI controller :(
}
