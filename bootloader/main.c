#include "efi_tools.h"
#include "guid_utils.h"
#include "boot_info.h"
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

    Print(L"--------------------------------------------------\n");

    wait_for_key();

    print_smbios_legacy();

    sleep_seconds(2);
    wait_for_key();

    return EFI_SUCCESS;
}
