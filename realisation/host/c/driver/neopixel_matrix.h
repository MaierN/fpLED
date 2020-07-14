
#ifndef NEOPIXEL_MATRIX_H
#define NEOPIXEL_MATRIX_H

#include <stdint.h>

#include "led_controller.h"

typedef struct {
    led_controller_t* lc;
    uint8_t channel;
    size_t offset;
    size_t x_size;
    size_t y_size;
} neopixel_matrix_t;

void neopixel_matrix_set_pixel_color(neopixel_matrix_t* nm, size_t x, size_t y, uint32_t color);

#endif
