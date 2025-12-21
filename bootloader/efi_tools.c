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

void *get_smbios_ptr() {
    EFI_GUID smbios_guid = SMBIOS_TABLE_GUID;
    for (UINTN i = 0; i < ST->NumberOfTableEntries; i++) {
        if (CompareGuid(&ST->ConfigurationTable[i].VendorGuid, &smbios_guid) == 0) {
            return ST->ConfigurationTable[i].VendorTable;
        }
    }
    return NULL;
}

void set_best_text_mode() {
    UINTN max_cols = 0;
    UINTN max_rows = 0;
    UINTN best_mode = 0;
    UINTN cols, rows;
    EFI_STATUS status;

    for (INTN i = 0; i < ST->ConOut->Mode->MaxMode; i++) {

        status = uefi_call_wrapper(ST->ConOut->QueryMode, 4, ST->ConOut, i, &cols, &rows);

        if (EFI_ERROR(status))
            continue;

        if ((cols * rows) > (max_cols * max_rows)) {
            max_cols = cols;
            max_rows = rows;
            best_mode = i;
        }
    }

    if (max_cols > 0) {
        uefi_call_wrapper(ST->ConOut->SetMode, 2, ST->ConOut, best_mode);
        uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
        Print(L"Switched to Text Mode %d (%dx%d)\n", best_mode, max_cols, max_rows);
    } else {
        Print(L"Could not find a better text mode.\n");
    }
}
