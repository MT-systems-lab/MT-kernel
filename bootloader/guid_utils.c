#include "guid_utils.h"

CHAR16* GetGuidName(EFI_GUID *Guid) {
    // Materialize the macros from efiapi.h and our header into variables
    static EFI_GUID Acpi2     = ACPI_20_TABLE_GUID;
    static EFI_GUID Acpi1     = ACPI_TABLE_GUID;
    static EFI_GUID Smbios    = SMBIOS_TABLE_GUID;
    static EFI_GUID Smbios3   = SMBIOS3_TABLE_GUID;

    // Compare and return the string
    if (CompareGuid(Guid, &Acpi2) == 0)   return L"ACPI 2.0 (RSDP)";
    if (CompareGuid(Guid, &Acpi1) == 0)   return L"ACPI 1.0 (RSDP)";
    if (CompareGuid(Guid, &Smbios) == 0)  return L"SMBIOS";
    if (CompareGuid(Guid, &Smbios3) == 0) return L"SMBIOS 3.0";

    CHAR16 buffer[40];
    SPrint(buffer, sizeof(buffer), L"%g", Guid);
    return StrDuplicate(buffer);
}
