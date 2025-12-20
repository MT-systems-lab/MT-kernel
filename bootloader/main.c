#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    uefi_call_wrapper(SystemTable->ConOut->Reset, 2, SystemTable->ConOut, FALSE);
    uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 2, SystemTable->ConOut, EFI_TEXT_ATTR(EFI_YELLOW, EFI_BLUE));
    uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
    uefi_call_wrapper(SystemTable->ConOut->OutputString, 2, SystemTable->ConOut, L"SUCCESS: Hello UEFI!\r\n");
    while(1) {}

    return EFI_SUCCESS;
}
