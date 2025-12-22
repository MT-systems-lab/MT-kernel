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

CHAR16 *GetGuidName(EFI_GUID *Guid) {
    static EFI_GUID Acpi2 = ACPI_20_TABLE_GUID;
    static EFI_GUID Acpi1 = ACPI_TABLE_GUID;
    static EFI_GUID Smbios = SMBIOS_TABLE_GUID;
    static EFI_GUID Smbios3 = SMBIOS3_TABLE_GUID;

    if (CompareGuid(Guid, &Acpi2) == 0)
        return L"ACPI 2.0 (RSDP)";
    if (CompareGuid(Guid, &Acpi1) == 0)
        return L"ACPI 1.0 (RSDP)";
    if (CompareGuid(Guid, &Smbios) == 0)
        return L"SMBIOS";
    if (CompareGuid(Guid, &Smbios3) == 0)
        return L"SMBIOS 3.0";

    CHAR16 buffer[40];
    SPrint(buffer, sizeof(buffer), L"%g", Guid);
    return StrDuplicate(buffer);
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

EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, void **entry_point) {
    EFI_STATUS status;
    EFI_FILE *root;
    EFI_FILE *kernel_file;
    EFI_LOADED_IMAGE *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

    uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol,
                      (void **)&loaded_image);

    uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &FileSystemProtocol,
                      (void **)&fs);

    uefi_call_wrapper(fs->OpenVolume, 2, fs, &root);

    uefi_call_wrapper(root->Open, 5, root, &kernel_file, L"kernel.bin", EFI_FILE_MODE_READ, 0);

    EFI_FILE_INFO *file_info;
    UINTN info_size = 0;

    uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &GenericFileInfo, &info_size, NULL);

    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, info_size, (void **)&file_info);

    uefi_call_wrapper(kernel_file->GetInfo, 4, kernel_file, &GenericFileInfo, &info_size,
                      file_info);

    UINTN file_size = file_info->FileSize;
    UINTN pages_needed = EFI_SIZE_TO_PAGES(file_size);

    EFI_PHYSICAL_ADDRESS kernel_addr = 0x100000;

    status = uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages_needed,
                               &kernel_addr);

    if (EFI_ERROR(status)) {
        Print(L"Error: Memory at 0x100000 is unavailable! %r\n", status);
        uefi_call_wrapper(BS->FreePool, 1, file_info);
        uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
        return status;
    }

    uefi_call_wrapper(kernel_file->Read, 3, kernel_file, &file_size, (void *)kernel_addr);

    uefi_call_wrapper(BS->FreePool, 1, file_info);
    uefi_call_wrapper(kernel_file->Close, 1, kernel_file);
    uefi_call_wrapper(root->Close, 1, root);

    *entry_point = (void *)kernel_addr;

    return EFI_SUCCESS;
}
