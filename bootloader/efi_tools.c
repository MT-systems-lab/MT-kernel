#include "efi_tools.h"

void sleep_seconds(UINTN seconds) {
    Print(L"Sleeping for %d seconds...\n", seconds);
    EFI_EVENT EventTimer;
    uefi_call_wrapper(BS->CreateEvent, 5, EVT_TIMER, TPL_CALLBACK, NULL, NULL, &EventTimer);
    uefi_call_wrapper(BS->SetTimer, 3, EventTimer, TimerRelative,
                      seconds * 10000000); // seconds to 100ns
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &EventTimer, NULL);
    uefi_call_wrapper(BS->CloseEvent, 1, EventTimer);
}

void wait_for_key() {
    Print(L"Press any key to continue...\n");
    UINTN Index;
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &Index);

    EFI_INPUT_KEY Key;
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &Key);
}

void printTimeDate() {
    EFI_TIME Time;
    EFI_STATUS Status;
    Status = uefi_call_wrapper(RT->GetTime, 2, &Time, NULL);
    if (EFI_ERROR(Status)) {
        Print(L"Error getting time: %r\n", Status);
        return;
    }
    Print(L"Current Time: %02d:%02d:%02d\n", Time.Hour, Time.Minute, Time.Second);
    Print(L"Date: %04d-%02d-%02d\n", Time.Year, Time.Month, Time.Day);
}

void *get_acpi_rsdp() {
    EFI_GUID acpi2_guid = ACPI_20_TABLE_GUID;

    for (UINTN i = 0; i < ST->NumberOfTableEntries; i++) {
        if (CompareGuid(&ST->ConfigurationTable[i].VendorGuid, &acpi2_guid) == 0) {
            return ST->ConfigurationTable[i].VendorTable;
        }
    }
    return NULL;
}

void* get_smbios_ptr() {
    EFI_GUID smbios_guid = SMBIOS_TABLE_GUID;
    for (UINTN i = 0; i < ST->NumberOfTableEntries; i++) {
        if (CompareGuid(&ST->ConfigurationTable[i].VendorGuid, &smbios_guid) == 0) {
            return ST->ConfigurationTable[i].VendorTable;
        }
    }
    return NULL;
}

