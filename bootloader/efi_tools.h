#ifndef EFI_TOOLS_H
#define EFI_TOOLS_H

#include <efi.h>
#include <efilib.h>

void sleep_seconds(UINTN seconds);
void wait_for_key();
void printTimeDate();
void *get_acpi_rsdp();
void* get_smbios_ptr();
void print_smbios_legacy();

#endif // EFI_TOOLS_H
