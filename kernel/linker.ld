ENTRY(kernel_main)

SECTIONS
{
    . = 1M;

    .text : {
        *(.entry)
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(COMMON)
        *(.bss)
    }
}
