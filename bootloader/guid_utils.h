#ifndef GUID_UTILS_H
#define GUID_UTILS_H

#include <efi.h>
#include <efilib.h>

CHAR16* GetGuidName(EFI_GUID* Guid);

#endif // GUID_UTILS_H
