#ifndef EFI_TOOLS_H
#define EFI_TOOLS_H


#include <efi.h>
#include <efilib.h>

void sleep_seconds(UINTN seconds);
void wait_for_key();
void printTimeDate();
CHAR16* GetGuidName(EFI_GUID* Guid);
void *get_acpi_rsdp();
void* get_smbios_ptr();
void print_smbios_legacy();
void set_best_text_mode();
EFI_STATUS load_kernel(EFI_HANDLE ImageHandle, void** entry_point);



#endif // EFI_TOOLS_H
