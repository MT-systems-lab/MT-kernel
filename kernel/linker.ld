ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text : {
        KEEP(*(.entry))
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
