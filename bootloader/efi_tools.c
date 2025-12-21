#include "efi_tools.h"

void sleep_seconds(UINTN seconds) {
    Print(L"Sleeping for %d seconds...\n", seconds);
    EFI_EVENT EventTimer;
    uefi_call_wrapper(BS->CreateEvent, 5, EVT_TIMER, TPL_CALLBACK, NULL, NULL, &EventTimer);
    uefi_call_wrapper(BS->SetTimer, 3, EventTimer, TimerRelative, seconds * 10000000); // seconds to 100ns
    uefi_call_wrapper(BS->WaitForEvent, 3, 1, &EventTimer, NULL);
    uefi_call_wrapper(BS->CloseEvent, 1, EventTimer);
}

