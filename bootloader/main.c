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

    EFI_TIME Time;
    EFI_STATUS Status;
    Status = uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);
    if (EFI_ERROR(Status)) {
        Print(L"Error getting time: %r\n", Status);
        return Status;
    }
    Print(L"Current Time: %02d:%02d:%02d\n", Time.Hour, Time.Minute, Time.Second);
    Print(L"Date: %04d-%02d-%02d\n", Time.Year, Time.Month, Time.Day);
    Print(L"--------------------------------------------------\n");

    Print(L"\n");
    Print(L"--- UEFI Configuration Table ---\n");
    Print(L"--------------------------------------------------\n");
    Print(L"| Address            | Type                      |\n");
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

    return EFI_SUCCESS;
}
