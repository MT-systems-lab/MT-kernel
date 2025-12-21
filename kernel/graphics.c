#include "../common/boot_info.h"
#include "../common/font.h"

static BootInfo *k_boot_info = 0;
static unsigned int cursor_x = 0;
static unsigned int cursor_y = 0;
static int FONT_SCALE = 2;

void init_graphics(BootInfo *boot_info) {
    k_boot_info = boot_info;
    cursor_x = 10;
    cursor_y = 10;
}

void put_pixel(unsigned int x, unsigned int y, unsigned int color) {
    if (x >= k_boot_info->Gpu.Width || y >= k_boot_info->Gpu.Height)
        return;

    unsigned int *fb = (unsigned int *)k_boot_info->Gpu.BaseAddress;
    unsigned int stride = k_boot_info->Gpu.PixelsPerScanLine;
    fb[y * stride + x] = color;
}

void draw_char(int x, int y, char c, unsigned int color, int scale) {
    if (c < 0 || c > 127)
        return;

    int index = c;

    for (int row = 0; row < 8; row++) {
        unsigned char bit_row = font8x8_basic[index][row];

        for (int col = 0; col < 8; col++) {
            if ((bit_row >> col) & 1) {
                for (int s_y = 0; s_y < scale; s_y++) {
                    for (int s_x = 0; s_x < scale; s_x++) {
                        put_pixel(x + (col * scale) + s_x, y + (row * scale) + s_y, color);
                    }
                }
            }
        }
    }
}

void kprint(const char *str) {
    unsigned int color = 0xFFFFFFFF;

    int char_height = 8 * FONT_SCALE;
    int char_width = 8 * FONT_SCALE;

    while (*str) {
        char c = *str;

        if (c == '\n') {
            cursor_x = 10;
            cursor_y += char_height + (2 * FONT_SCALE);
            str++;
            continue;
        }

        draw_char(cursor_x, cursor_y, c, color, FONT_SCALE);

        cursor_x += char_width;

        if (cursor_x >= k_boot_info->Gpu.Width - char_width) {
            cursor_x = 10;
            cursor_y += char_height + (2 * FONT_SCALE);
        }

        str++;
    }
}
