#include "../common/boot_info.h"

__attribute__((section(".text"))) void kernel_entry(BootInfo *boot_info) {

    unsigned int *fb = (unsigned int *)boot_info->Gpu.BaseAddress;

    unsigned long total_pixels = boot_info->Gpu.BufferSize / 4;

    for (unsigned long i = 0; i < total_pixels; i++) {
        fb[i] = 0xFF000000;
    }

    unsigned long bar_size = boot_info->Gpu.PixelsPerScanLine * 200;

    for (unsigned long i = 0; i < bar_size; i++) {
        fb[i] = 0xFF00FF00;
    }

    unsigned int x_start = boot_info->Gpu.Width / 2;
    unsigned int y_start = boot_info->Gpu.Height / 2;
    unsigned int stride = boot_info->Gpu.PixelsPerScanLine;

    for (unsigned int y = 0; y < 100; y++) {
        for (unsigned int x = 0; x < 100; x++) {
            unsigned long index = ((y_start + y) * stride) + (x_start + x);
            fb[index] = 0xFFFF0000;
        }
    }

    while (1)
        __asm__("hlt");
}
