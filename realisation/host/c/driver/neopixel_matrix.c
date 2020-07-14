
#include "neopixel_matrix.h"

void neopixel_matrix_set_pixel_color(neopixel_matrix_t* nm, size_t x, size_t y, uint32_t color) {
    nm->lc->channels[nm->channel].leds[nm->offset + y * nm->x_size + (y % 2 == 0 ? x : nm->x_size - 1 - x)] = color;
}
