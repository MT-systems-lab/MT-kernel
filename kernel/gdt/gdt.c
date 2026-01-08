#include "gdt.h"

__attribute__((aligned(0x1000))) struct GDT gdt = {
    .NullSegment = {0, 0, 0, 0, 0, 0},
    .KernelCodeSegment = {0, 0, 0, 0x9A, 0xA0, 0},
    .KernelDataSegment = {0, 0, 0, 0x92, 0xA0, 0},
    .UserNullSegment = {0, 0, 0, 0, 0, 0},
    .UserCodeSegment = {0, 0, 0, 0x9A, 0xA0, 0},
    .UserDataSegment = {0, 0, 0, 0x92, 0xA0, 0},
};
