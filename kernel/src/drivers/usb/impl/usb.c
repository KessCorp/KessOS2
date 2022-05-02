#include <drivers/usb/usb.h>
#include <drivers/usb/host/uhci.h>
#include <drivers/usb/host/xhci.h>
#include <debug/log.h>
#include <stdint.h>


void usb_init() {
    log("Searching for available USB controllers..\n", S_INFO); 

    if (uhci_init()) {
        log("Found controller for USB 1.0 (UHCI) (Ignoring).\n", S_INFO);
        log("I am looking for someone who sees this message, if so, contact me at [HSC] Spurious#0070 or the username in uhci.c comment.\n", S_INFO); 
    }


    if (xhci_init()) {
        log("Found XHCI controller (Putting to use).\n", S_INFO);
        return;
    } else {
        // TODO: Remove this else when adding more controllers.
        log("Found no USB controller, USB will not work!", S_INFO);
    }
}
