#include "boot_info.h"
#include "efi_tools.h"
#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    uefi_call_wrapper(ST->ConOut->Reset, 2, ST->ConOut, FALSE);
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut,
                      EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));

    InitializeLib(ImageHandle, SystemTable);
    set_best_text_mode();

    Print(L"-------------------------------\n");
    Print(L"|     MT GNU-EFI Bootloader   |\n");
    Print(L"-------------------------------\n");

    printTimeDate();

    Print(L"--------------------------------------------------\n");
    Print(L"--- UEFI Configuration Table ---\n");
    Print(L"--------------------------------------------------\n");
    EFI_CONFIGURATION_TABLE *ConfigTable = ST->ConfigurationTable;
    UINTN Entries = ST->NumberOfTableEntries;

    for (UINTN i = 0; i < Entries; i++) {
        Print(L"| ");
        Print(L"Table %d: %s at %p\n", i, GetGuidName(&ConfigTable[i].VendorGuid),
              ConfigTable[i].VendorTable);
    }
    Print(L"--------------------------------------------------\n");

    BootInfo *b_info;
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(BootInfo), (void **)&b_info);
    uefi_call_wrapper(BS->SetMem, 3, b_info, sizeof(BootInfo), 0);

    VOID *Rsdp = get_acpi_rsdp();
    b_info->AcpiRsdp = Rsdp;
    Print(L"ACPI RSDP 2.0 Address: %p\n", Rsdp);

    VOID *SmbiosPtr = get_smbios_ptr();
    b_info->SmbiosInfo = SmbiosPtr;
    Print(L"SMBIOS Entry Point Address: %p\n", SmbiosPtr);

    b_info->RuntimeServices = ST->RuntimeServices;

    Print(L"--------------------------------------------------\n");

    // ------------------------------
    // framebuffer
    // ------------------------------

    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;

    uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void **)&Gop);
    b_info->Gpu.BaseAddress = (uint64_t)(UINTN)Gop->Mode->FrameBufferBase;
    b_info->Gpu.BufferSize = (uint64_t)Gop->Mode->FrameBufferSize;
    b_info->Gpu.Width = Gop->Mode->Info->HorizontalResolution;
    b_info->Gpu.Height = Gop->Mode->Info->VerticalResolution;
    b_info->Gpu.PixelsPerScanLine = Gop->Mode->Info->PixelsPerScanLine;
    b_info->Gpu.PixelFormat = (uint8_t)Gop->Mode->Info->PixelFormat;

    Print(L"GOP found: %dx%d at 0x%lx with size: %d\n", b_info->Gpu.Width, b_info->Gpu.Height,
          b_info->Gpu.BaseAddress, b_info->Gpu.BufferSize);

    Print(L"--------------------------------------------------\n");

    // ------------------------------
    // Kernel loading
    // ------------------------------
    void *kernel_entry = NULL;
    Print(L"Loading kernel...\n");
    EFI_STATUS status = load_kernel(ImageHandle, &kernel_entry);
    if (EFI_ERROR(status)) {
        Print(L"Failed to load kernel: %r\n", status);
        wait_for_key();
        return status;
    }
    Print(L"Kernel loaded at entry point: %p\n", kernel_entry);
    Print(L"--------------------------------------------------\n");

    // ------------------------------
    // SMBIOS
    // ------------------------------
    print_smbios_legacy();
    // sleep_seconds(1);
    wait_for_key();

    // ------------------------------
    // memory map
    // ------------------------------
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, NULL, &MapKey, &DescriptorSize,
                      &DescriptorVersion);
    MemoryMapSize += 8 * DescriptorSize;
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MemoryMapSize, (void **)&MemoryMap);

    uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize,
                      &DescriptorVersion);
    uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, MapKey);

    uint32_t *fb = (uint32_t *)b_info->Gpu.BaseAddress;
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            fb[i * b_info->Gpu.PixelsPerScanLine + j] = 0xFFFFFFFF; // White
        }
    }

    b_info->MemInfo.MemoryMap = MemoryMap;
    b_info->MemInfo.MapSize = MemoryMapSize;
    b_info->MemInfo.DescriptorSize = DescriptorSize;

    // ------------------------------
    // Jump to kernel
    // ------------------------------
    typedef void (*KernelStartFunc)(BootInfo *);
    KernelStartFunc kernel_main = (KernelStartFunc)kernel_entry;
    kernel_main(b_info);

    while (1)
        ;
    return EFI_SUCCESS;
}
