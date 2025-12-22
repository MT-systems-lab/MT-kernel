#include <efi.h>
#include <efilib.h>

void print_smbios_legacy() {
    EFI_STATUS status;
    SMBIOS_STRUCTURE_TABLE *smbios_table = NULL;
    SMBIOS_STRUCTURE_POINTER smbios;
    SMBIOS_STRUCTURE_POINTER smbios_end;
    UINT16 index;

    status = LibGetSystemConfigurationTable(&SMBIOSTableGuid, (VOID **)&smbios_table);

    if (EFI_ERROR(status)) {
        Print(L"Error: Legacy SMBIOS Table not found.\n");
        return;
    }

    smbios.Hdr = (SMBIOS_HEADER *)(uintptr_t)smbios_table->TableAddress;
    smbios_end.Raw = (UINT8 *)((uintptr_t)smbios_table->TableAddress + smbios_table->TableLength);

    for (index = 0; index < smbios_table->NumberOfSmbiosStructures; index++) {


        // --- TYPE 1: System Information (Motherboard) ---
        if (smbios.Hdr->Type == 1) {
            Print(L"[System]\n");
            Print(L"  Maker:   %a\n", LibGetSmbiosString(&smbios, smbios.Type1->Manufacturer));
            Print(L"  Product: %a\n", LibGetSmbiosString(&smbios, smbios.Type1->ProductName));
        }

        // --- TYPE 4: Processor Information (CPU) ---
        else if (smbios.Hdr->Type == 4) {
            Print(L"\n[CPU]\n");
            Print(L"  Model:   %a\n", LibGetSmbiosString(&smbios, smbios.Type4->ProcessorVersion));
            Print(L"  Socket:  %a\n", LibGetSmbiosString(&smbios, smbios.Type4->Socket));
        }


        LibGetSmbiosString(&smbios, -1);

        if (smbios.Raw >= smbios_end.Raw) {
            break;
        }
    }
    Print(L"--------------------------------------------------\n");
}
