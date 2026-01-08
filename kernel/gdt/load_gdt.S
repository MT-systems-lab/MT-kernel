.code64

.global LoadGDT
LoadGDT:
    lgdt (%rdi)           # Load the GDT from the address in RDI
    movw $0x10, %ax       # kernel Data segment selector
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss
    popq %rdi
    movq $0x08, %rax      # kernel Code segment selector
    pushq %rax
    pushq %rdi
    lretq
