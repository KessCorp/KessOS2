#include <arch/memory/memory.h>


const char* const MSEGMENTS[] = {
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode"
};


uint64_t getMemSize(memdesc_t* mMap, uint64_t mEntryCount, uint64_t mDescSize) {
    uint64_t memSizeBytes = 0;

    for (int entry = 0; entry < mEntryCount; ++entry) {
        memdesc_t* desc = (memdesc_t*)((uint64_t)mMap + (entry + mDescSize));
        memSizeBytes += desc->nPages * 4096;
    }

    return memSizeBytes;
}



uint64_t get_mmap_entries(meminfo_t meminfo) {
    return meminfo.mSize / meminfo.mDescriptorSize;
}


memdesc_t* mmap_iterator_helper(uint64_t i, meminfo_t meminfo) {
    return (memdesc_t*)((int64_t)meminfo.mMap + (i * (meminfo.mDescriptorSize)));
}
