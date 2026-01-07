#include "../common/boot_info.h"

void init_graphics(BootInfo *boot_info);
void kprint(const char *str);

// __attribute__((section(".entry"))) 
void kernel_main(BootInfo *boot_info) {

    init_graphics(boot_info);
    unsigned int *fb = (unsigned int *)boot_info->Gpu.BaseAddress;

    for (unsigned long i = 0; i < boot_info->Gpu.BufferSize / 4; i++)
        fb[i] = 0xFF0000FF;

    kprint("MTOS> Hello, Kernel!\n");
    kprint("MTOS> Graphics Initialized.");
    for (int i = 0; i < 50; i++)
        kprint("MTOS> Line number\n");

    while (1)
        __asm__("hlt");
}
