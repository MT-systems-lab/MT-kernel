.code64
.intel_syntax noprefix

.global LoadGDT
LoadGDT:
    lgdt [rdi]  # Load the GDT from the address in RDI
    mov ax, 0x10 # kernel Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi
    mov rax, 0x08 # kernel Code segment selector
    push rax
    push rdi
    retfq
