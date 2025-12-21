#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *ST) {
    uefi_call_wrapper(ST->ConOut->Reset, 2, ST->ConOut, FALSE);
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));

    InitializeLib(ImageHandle, ST);
    Print(L"Hello, world!\n");

    while (1) {
        __asm__ __volatile__("hlt");
    }

    return EFI_SUCCESS;
}
