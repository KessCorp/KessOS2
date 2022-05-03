#include <firmware/acpi/acpi.h>
#include <firmware/acpi/tables.h>
#include <debug/log.h>
#include <util/asm.h>
#include <util/string.h>


// 2022 Ian Moffett <ian@kesscoin.com>


static acpi_rsdt_t* rsdt = NULL;
static unsigned int rsdt_entries = 0;
static acpi_fadt_t* fadt = NULL;


static uint8_t rsdt_valid() {
    unsigned char sum = 0;

    for (int i = 0; i < rsdt->header.length; ++i) {
        char* tmp = (char*)&rsdt->header;
        sum += tmp[i];
    }

    return sum % 0x100 == 0;
}


static uint8_t locate_fadt() {
    for (int i = 0; i < rsdt_entries; ++i) {
        acpi_header_t* current_header = (acpi_header_t*)(uint64_t)rsdt->tables[i];
        if (strncmp(current_header->signature, "FACP", 4)) {
            fadt = (acpi_fadt_t*)(uint64_t)rsdt->tables[i];
            return 1;
        }
    }

    return 0;
}


static uint8_t is_fadt_valid() {
    unsigned char sum = 0;

    for (int i = 0; i < fadt->header.length; ++i) {
        char* tmp = (char*)&fadt->header;
        sum += tmp[i];
    }

    return sum % 0x100 == 0;
}


void acpi_init(void* rsdp) {
    log("Setting up ACPI..\n", S_INFO);

    acpi_rsdp_t* rsdp_struct = (acpi_rsdp_t*)rsdp;

    rsdt = (acpi_rsdt_t*)(uint64_t)rsdp_struct->rsdtaddr;

    if (!(rsdt_valid)) {
        // Something funky happened if RSDT checksum goes invalid.
        log("RSDT checksum invalid, please contact me at [HSC] Spurious#0070 on Discord (System Halted Until Reboot).\n", S_CRITICAL);
        HALT;
    }

    // Get amount of RSDT entries.
    rsdt_entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

    // Locate FADT.    
    if (!(locate_fadt())) {
        log("FADT not found (System Halted Until Reboot).\n", S_CRITICAL);
        HALT;
    }

    // Check if FADT is valid.
    if (!(is_fadt_valid())) {
        log("FADT checksum invalid, please contact me at [HSC] Spurious#0070 on Discord (System Halted Until Reboot).\n", S_CRITICAL);
        HALT;
    }
}
