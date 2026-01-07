.code64
.section .entry
.global _start
.extern kernel_main

_start:
    cli                         // Clear interrupts
    movq    $stack_top, %rsp    // Set up stack pointer

    call    kernel_main         // Call the kernel main function
    cli

.hang:
    hlt                         // Halt the CPU
    jmp     .hang               // Infinite loop

.section .bss
.align 16
stack_bottom:
    .space 0x4000               // 16KB stack
stack_top:
