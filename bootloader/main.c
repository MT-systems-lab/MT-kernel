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
              &ConfigTable[i].VendorTable);
    }
    Print(L"--------------------------------------------------\n");

    sleep_seconds(2);
    wait_for_key();


    return EFI_SUCCESS;
}
