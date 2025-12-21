#include "boot_info.h"
#include "efi_tools.h"
#include "guid_utils.h"
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

    Print(L"\n");
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
    // memory map
    // ------------------------------
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, NULL, &MapKey, &DescriptorSize,
                      &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, MemoryMapSize, (void **)&MemoryMap);
    uefi_call_wrapper(BS->GetMemoryMap, 5, &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize,
                      &DescriptorVersion);

    b_info->MemInfo.MemoryMap = MemoryMap;
    b_info->MemInfo.MapSize = MemoryMapSize;
    b_info->MemInfo.MapKey = MapKey;
    b_info->MemInfo.DescriptorSize = DescriptorSize;
    b_info->MemInfo.DescriptorVersion = DescriptorVersion;
    Print(L"Memory Map Address: %p\n", MemoryMap);
    Print(L"Memory Map Size: %lu bytes\n", MemoryMapSize);
    Print(L"Descriptor Size: %u bytes\n", DescriptorSize);

    UINT32 MEMType = MemoryMap->Type;
    EFI_PHYSICAL_ADDRESS PhysicalStart = MemoryMap->PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart = MemoryMap->VirtualStart;
    UINT64 NumberOfPages = MemoryMap->NumberOfPages;
    Print(L"Memory Type: %u\n", MEMType);
    Print(L"Physical Start: %p\n", PhysicalStart);
    Print(L"Virtual Start: %p\n", VirtualStart);
    Print(L"Number Of 4KiB Pages: %lu\n", NumberOfPages);

    Print(L"\nMemory Map Succesfully Retrieved.\n");
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

    Print(L"GOP found: %dx%d at 0x%lx\n", b_info->Gpu.Width, b_info->Gpu.Height,
          b_info->Gpu.BaseAddress);

    wait_for_key();

    print_smbios_legacy();

    sleep_seconds(2);
    wait_for_key();

    return EFI_SUCCESS;
}
