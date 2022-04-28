#include <efi.h>
#include <efilib.h>
#include <stddef.h>
#include <elf.h>

#define PSF1_MAGIC0 0x00000036
#define PSF1_MAGIC1 0x00000004

#define BACKGROUND_COLOR 0x0000FF
#define TOP_TEXT "KessBoot v1.4 -- use up/down arrows to move and right arrow to select. Or left arrow for legacy support."

typedef struct {
  void* baseAddr;
  size_t bufferSize;
  unsigned int width;
  unsigned int height;
  unsigned int ppsl;      // Pixels per scanline.
} framebuffer_t;


typedef struct {
  unsigned char magic[2];
  unsigned char mode;
  unsigned char chsize;
} psf1_header_t;


typedef struct {
  psf1_header_t* header;
  void* glyphBuffer;
} psf1_font_t;


typedef struct {
  EFI_MEMORY_DESCRIPTOR* mMap;
  UINTN mSize;
  UINTN mDescriptorSize;
} meminfo_t;


framebuffer_t* initGOP(EFI_SYSTEM_TABLE* sysTable) {
  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
  EFI_STATUS s = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);

  if (EFI_ERROR(s)) {
    return NULL;
  }

  framebuffer_t* fb;
  sysTable->BootServices->AllocatePool(EfiLoaderData, sizeof(framebuffer_t), (void**)&fb);
  fb->baseAddr = (void*)gop->Mode->FrameBufferBase;
  fb->bufferSize = gop->Mode->FrameBufferSize;
  fb->width = gop->Mode->Info->HorizontalResolution;
  fb->height = gop->Mode->Info->VerticalResolution;
  fb->ppsl = gop->Mode->Info->PixelsPerScanLine;
  return fb;
}


int memcmp(const void* aptr, const void* bptr, size_t n) {
  const unsigned char* a = aptr, *b = bptr;
  for (size_t i = 0; i < n; i++) {
    if (a[i] < b[i]) return -1;
    else if (a[i] > b[i]) return 1;
  }
  return 0;
}


EFI_FILE* loadFile(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
  EFI_FILE* fileRes;
  EFI_LOADED_IMAGE_PROTOCOL* loadedImage;
  sysTable->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (void**)&loadedImage);
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
  sysTable->BootServices->HandleProtocol(loadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&fs);

  if (!(dir)) {
    fs->OpenVolume(fs, &dir);
  }

  EFI_STATUS s = dir->Open(dir, &fileRes, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

  if (s != EFI_SUCCESS) {
    return NULL;
  }

  return fileRes;
}


psf1_font_t* loadFont(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
  EFI_FILE* font = loadFile(dir, path, imageHandle, sysTable);

  if (!(font)) {
    Print(L"Font file not found!\n");
    return NULL;
  }

  psf1_header_t* fontHeader;
  sysTable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_header_t), (void**)&fontHeader);
  UINTN size = sizeof(psf1_header_t);
  font->Read(font, &size, fontHeader);

  if (!(fontHeader->magic[0] & PSF1_MAGIC0) || !(fontHeader->magic[1] & PSF1_MAGIC1)) {
    Print(L"Font header invalid!\n");
    return NULL;
  }

  UINTN glyphBufferSize = fontHeader->chsize * 255;

  if (fontHeader->mode == 1) {
    glyphBufferSize = fontHeader->chsize * 512;
  }

  void* glyphBuffer;
  font->SetPosition(font, sizeof(psf1_header_t));
  sysTable->BootServices->AllocatePool(EfiLoaderData, glyphBufferSize, (void**)&glyphBuffer);
  font->Read(font, &glyphBufferSize, glyphBuffer);

  psf1_font_t* fontRes;
  sysTable->BootServices->AllocatePool(EfiLoaderData, sizeof(psf1_font_t), (void**)&fontRes);
  fontRes->header = fontHeader;
  fontRes->glyphBuffer = glyphBuffer;
  return fontRes;
}


void clear_screen(framebuffer_t* lfb, uint64_t color) {
  uint64_t bytesPerScanline = lfb->ppsl * 4;

  for (uint64_t vsl = 0; vsl < lfb->height; ++vsl) {
    uint64_t pixPtrBase = (uint64_t)lfb->baseAddr + (bytesPerScanline * vsl);
    for (uint32_t* pixelPtr = (uint32_t*)pixPtrBase; pixelPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); ++pixelPtr) {
      *pixelPtr = color;
    }
  }
}


void putChar(framebuffer_t* framebuffer, psf1_font_t* psf1_font, unsigned int color, char chr, unsigned int xOff, unsigned int yOff) {
  unsigned int* pixPtr = (unsigned int*)framebuffer->baseAddr;
  char* fontPtr = psf1_font->glyphBuffer + (chr * psf1_font->header->chsize);
  for (unsigned long y = yOff; y < yOff + 16; y++){
    for (unsigned long x = xOff; x < xOff+8; x++){
      if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
        *(unsigned int*)(pixPtr + x + (y * framebuffer->ppsl)) = color;
      }

    }
    fontPtr++;
  }
}


size_t strlen(const char* const STR) {
  size_t len = 0;
  while (STR[len++]);
  return len;
}


void menu_select_boot(framebuffer_t* lfb, psf1_font_t* font) {
  clear_screen(lfb, BACKGROUND_COLOR);
  const uint64_t LIMIT = ((lfb->height / 2) - 245) + 15;
  const uint64_t START = (lfb->height / 2) - 245;
  const char* const BOOT_OPTION = "BOOT [X]";
  const char* const REBOOT_OPTION = "REBOOT []";

  size_t idx = 0;

  // Display loader name.
  uint64_t x = START + 100;
  while (TOP_TEXT[idx]) {
    putChar(lfb, font, 0x000000, TOP_TEXT[idx], x, 50);
    x += 8;
    ++idx;
  }

  // Display boot option.
  for (uint64_t x = 0; x < lfb->width; ++x) {
    unsigned int* pixPtr = (unsigned int*)lfb->baseAddr;
    for (uint64_t y = START; y < LIMIT; ++y) {
      *(unsigned int*)(pixPtr + x + (y * lfb->ppsl)) = 0xFFFFFF;
        }
  }

  idx = 0;
    x = START + 350;

    while (BOOT_OPTION[idx]) {
      putChar(lfb, font, 0x000000, BOOT_OPTION[idx], x, START);
      x += 8;
      ++idx;
    }

    // Display reboot option.
    for (uint64_t x = 0; x < lfb->width; ++x) {
      unsigned int* pixPtr = (unsigned int*)lfb->baseAddr;
      for (uint64_t y = START + 50; y < (START + 50) + 15; ++y) {
        *(unsigned int*)(pixPtr + x + (y * lfb->ppsl)) = 0x808080;
      }
    }

    idx = 0;
    x = START + 350;

    while (REBOOT_OPTION[idx]) {
      putChar(lfb, font, 0x000000, REBOOT_OPTION[idx], x, START + 50);
      x += 8;
      ++idx;
    }
}


void menu_select_reboot(framebuffer_t* lfb, psf1_font_t* font) {
  clear_screen(lfb, BACKGROUND_COLOR);
  const uint64_t LIMIT = ((lfb->height / 2) - 245) + 15;
  const uint64_t START = (lfb->height / 2) - 245;
  const char* const BOOT_OPTION = "BOOT []";
  const char* const REBOOT_OPTION = "REBOOT [X]";

  size_t idx = 0;

  // Display loader name.
  uint64_t x = START + 100;
  while (TOP_TEXT[idx]) {
    putChar(lfb, font, 0x000000, TOP_TEXT[idx], x, 50);
    x += 8;
    ++idx;
  }

  // Dislay boot option.
  for (uint64_t x = 0; x < lfb->width; ++x) {
    unsigned int* pixPtr = (unsigned int*)lfb->baseAddr;
    for (uint64_t y = START; y < LIMIT; ++y) {
      *(unsigned int*)(pixPtr + x + (y * lfb->ppsl)) = 0x808080;
    }
  }

  idx = 0;
  x = START + 350;

  while (BOOT_OPTION[idx]) {
    putChar(lfb, font, 0x000000, BOOT_OPTION[idx], x, START);
    x += 8;
    ++idx;
  }

  // Display reboot option.
  for (uint64_t x = 0; x < lfb->width; ++x) {
    unsigned int* pixPtr = (unsigned int*)lfb->baseAddr;
    for (uint64_t y = START + 50; y < (START + 50) + 15; ++y) {
      *(unsigned int*)(pixPtr + x + (y * lfb->ppsl)) = 0xFFFFFF;
    }
  }

  idx = 0;
  x = START + 350;

  while (REBOOT_OPTION[idx]) {
    putChar(lfb, font, 0x000000, REBOOT_OPTION[idx], x, START + 50);
    x += 8;
    ++idx;
  }
}




EFI_STATUS efi_main(EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* sysTable) {
  InitializeLib(imageHandle, sysTable);

  EFI_FILE* kernel = loadFile(NULL, L"kernel.elf", imageHandle, sysTable);

  if (!(kernel)) {
    Print(L"Kernel does not exist!\n");
  } else {
    Elf64_Ehdr header;
    UINTN fileInfoSize;
    kernel->GetInfo(kernel, &gEfiFileInfoGuid, &fileInfoSize, NULL);
    UINTN size = sizeof(header);
    kernel->Read(kernel, &size, &header);

    if (memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
        header.e_ident[EI_CLASS] != ELFCLASS64 ||
        header.e_type != ET_EXEC ||
        header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT) {
      Print(L"Kernel ELF header bad!\n");
    } else {
      Elf64_Phdr* progHeaders;
      kernel->SetPosition(kernel, header.e_phoff);
      UINTN progHeaderSize = header.e_phnum * header.e_phentsize;
      sysTable->BootServices->AllocatePool(EfiLoaderData, progHeaderSize, (void**)&progHeaders);
      kernel->Read(kernel, &progHeaderSize, progHeaders);

      for (Elf64_Phdr* phdr = progHeaders; (char*)phdr < (char*)progHeaders + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)) {
        if (phdr->p_type == PT_LOAD) {
          int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
          Elf64_Addr segment = phdr->p_paddr;
          sysTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
          kernel->SetPosition(kernel, phdr->p_offset);
          UINTN size = phdr->p_filesz;
          kernel->Read(kernel, &size, (void*)segment);
        }
      }

      framebuffer_t* lfb = initGOP(sysTable);
      psf1_font_t* font = loadFont(NULL, L"zap-light16.psf", imageHandle, sysTable);

      // Draw menu.
      menu_select_boot(lfb, font);

      // Let the boot menu do its thing.
      EFI_INPUT_KEY Key;

      uint8_t menuEntry = 1;      // BOOT will be 1 and REBOOT will  be 0.
      uint8_t loop = 1;
      uint8_t legacy_boot = 0;    // If 1 then legacy support is enabled.

      while (loop) {
        ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
        

        if (Key.ScanCode == 2 && menuEntry) {
          menu_select_reboot(lfb, font);
          menuEntry = 0;
        } else if (Key.ScanCode == 1 && !(menuEntry)) {
          menu_select_boot(lfb, font);
          menuEntry = 1;
        } else if (Key.ScanCode == 3) {
          switch (menuEntry) {
          case 1:
            loop = 0;
            continue;
          case 0:
            __asm__("out %%al, %%dx" : :"a" (0xFE), "d" (0x64));
            break;
          }
        } else if (Key.ScanCode == 4 && menuEntry) {
          loop = 0;
          legacy_boot = 1;
          continue;
        }
      }

      clear_screen(lfb, 0x000000);


      EFI_MEMORY_DESCRIPTOR* map = NULL;

      UINTN mapSize, mapKey, descriptorSize;
      UINT32 descriptorVersion;

      sysTable->BootServices->GetMemoryMap(&mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);
      sysTable->BootServices->AllocatePool(EfiLoaderData, mapSize, (void**)&map);
      sysTable->BootServices->GetMemoryMap(&mapSize, map, &mapKey, &descriptorSize, &descriptorVersion);

      meminfo_t mem_info = {
        .mMap = map,
        .mSize = mapSize,
        .mDescriptorSize = descriptorSize
      };

      // Look for RSDP.
      EFI_CONFIGURATION_TABLE* configTable = sysTable->ConfigurationTable;
      EFI_GUID rsdp_guid = ACPI_20_TABLE_GUID;
      unsigned long long* rsdp = NULL;

      for (UINTN i = 0; i < sysTable->NumberOfTableEntries; ++i) {
        if (CompareGuid(&configTable[i].VendorGuid, &rsdp_guid)) {
          if (memcmp((char*)"RSD PTR ", (char*)configTable->VendorTable, 8) == 0) {
            rsdp = (void*)configTable->VendorTable;
          }
        }

        ++configTable;
      }

      void(*kernel_entry)(framebuffer_t*, psf1_font_t*, meminfo_t, void*, uint8_t) = ((__attribute__((sysv_abi))void(*)(framebuffer_t*, psf1_font_t*, meminfo_t, void*, uint8_t))header.e_entry);

      if (font) {
        sysTable->BootServices->ExitBootServices(imageHandle, mapKey);
        kernel_entry(lfb, font, mem_info, (void*)rsdp, legacy_boot);
      }
    }
  }

  return EFI_SUCCESS;
}
