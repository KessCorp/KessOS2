#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include <stdint.h>

#define PAGE_SIZE 4096

typedef enum {
    MMAP_RESERVED,
    MMAP_EFI_LOADER_CODE,
    MMAP_EFI_LOADER_DATA,
    MMAP_EFI_BOOTSERVICES_CODE,
    MMAP_EFI_BOOTSERVICES_DATA,
    MMAP_EFI_RUNTIME_SERVICES_CODE,
    MMAP_EFI_RUNTIME_SERVICES_DATA,
    MMAP_USABLE_MEMORY,
    MMAP_UNUSABLE_MEMORY,
    MMAP_ACPI_RECLAIM_MEMORY,
    MMAP_ACPI_MEMORY_NVS,
    MMAP_MEMORY_MAPPED_IO,
    MMAP_MEMORY_MAPPED_IO_PORT_SPACE,
    MMAP_EFI_PAL_CODE,
} mem_type_t;


typedef struct {
    uint32_t type;
    void* physAddr;
    void* virtAddr;
    uint64_t nPages;
    uint64_t attr;
} memdesc_t;


typedef struct {
    memdesc_t* mMap;
    uint64_t mSize;
    uint64_t mDescriptorSize;
} meminfo_t;

uint64_t getMemSize(memdesc_t* mMap, uint64_t mEntryCount, uint64_t mDescSize);
uint64_t get_mmap_entries(meminfo_t meminfo);
memdesc_t* mmap_iterator_helper(uint64_t i, meminfo_t meminfo);

extern const char* const MSEGMENTS[];

#endif
