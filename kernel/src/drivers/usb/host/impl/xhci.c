#include <drivers/usb/host/xhci.h>
#include <drivers/pci/pci.h>
#include <debug/log.h>
#include <util/asm.h>

// 2022 Ian Moffett <ian@kesscoin.com>


static struct PCIDevice xhci_controller_info;

static volatile struct Controller {
    volatile struct CapRegs {
        uint8_t caplength;
        uint8_t rsvd;
        uint16_t hciversion;
        uint32_t hcsparams1;
        uint32_t hcsparams2;
        uint32_t hccparams1;
        uint32_t dboff;
        uint32_t rtsoff;
        uint32_t hccparams2;
    } *cap_regs;
    
    volatile struct OpRegs {
        uint32_t usbcmd;
        uint32_t usbsts;
        uint64_t dnctrl;
        uint32_t crcr;
        uint64_t dcbaap;
        uint64_t config;
    } *op_regs;

    volatile struct PortRegs {
        uint32_t portsc;
        uint32_t portpmsc;
        uint32_t portli;
        uint32_t reserved;
    } *port_regs;

    uint64_t mmio_addr;
} controller;


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

                    xhci_controller_info = xhci;
                    uint32_t bar0 = pci_get_bar0(bus, slot, func);
                    uint32_t bar1 = pci_get_bar1(bus, slot, func);
                    
                    // Setup controller regs.
                    controller.mmio_addr = (uint64_t)bar1 << 32 | bar0;
                    controller.cap_regs = (void*)controller.mmio_addr;
                    controller.op_regs = (void*)controller.mmio_addr + controller.cap_regs->caplength;

                    // Log some stuff.
                    log("xHCI MMIO => %x\n", S_INFO, controller.mmio_addr);

                    // Check if crcr is zero.
                    if (controller.op_regs->crcr != 0) {
                        log("Controller's Operational Register's crcr != 0 (System Halted Until Reboot)\n", S_CRITICAL);
                        HALT;
                    }

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



void xhci_reset() {
    controller.op_regs->usbcmd |= (1 << 1);
}
