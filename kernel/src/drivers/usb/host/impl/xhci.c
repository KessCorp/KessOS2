#include <drivers/usb/host/xhci.h>
#include <drivers/pci/pci.h>
#include <debug/log.h>
#include <util/asm.h>
#include <arch/memory/vmm.h>

// 2022 Ian Moffett <ian@kesscoin.com>


static struct PCIDevice xhci_controller_info;

static volatile struct Controller { 
    volatile struct CapRegs {
        uint8_t caplen;
        uint8_t reserved;
        uint16_t hciversion;
        uint32_t hcsparams1;
        uint32_t hcsparmas2;
        uint32_t hcsparams3;
        uint32_t hccparams1;
        uint32_t dboff;
        uint32_t rtsoff;
        uint32_t hccparams2;
    } *cap_regs;

    __attribute__((aligned(4))) volatile struct OpRegs {
        uint32_t usbcmd;
        uint32_t usbsts;
        uint32_t pagesz;
        uint64_t unused;
    } *op_regs;

    uint64_t mmio_addr;
} controller;


uint8_t xhci_init() {
    for (int bus = 0; bus < PCI_HIGHEST_BUS; ++bus) {
        for (int slot = 0; slot < PCI_HIGHEST_SLOT; ++slot) {
            for (int func = 0; func < PCI_HIGHEST_FUNC; ++func) {
                uint16_t classid = pci_get_classid(bus, slot, func);
                uint8_t subclass_id = pci_get_subclass_id(bus, slot, func);
                uint8_t interface_byte = pci_get_prog_if(bus, slot, func);

                if (classid == 0x0C && subclass_id == 0x03 && interface_byte == 0x30) {
                    xhci_controller_info.bus = bus;
                    xhci_controller_info.slot = slot;
                    xhci_controller_info.func = func;

                    controller.mmio_addr = (uint64_t)pci_get_bar1(bus, slot, func) << 32 | pci_get_bar0(bus, slot, func);
                    
                    // Map page starting from controller.mmio_addr
                    map_page(controller.mmio_addr);
                    log("Mapped MMIO address with default flags\n", S_INFO);

                    // Setup needed reg-spaces.
                    controller.cap_regs = (volatile void*)controller.mmio_addr;
                    controller.op_regs = (volatile void*)controller.mmio_addr + controller.cap_regs->caplen;

                    log("xHCI MMIO => %x\n", S_INFO, controller.mmio_addr);
                    return 1;           // xHCI controller found!
                }
            }
        }
    }

    return 0;   // No XHCI controller found.
}



void xhci_reset() {
    controller.op_regs->usbcmd |= (1 << 1);
}
