
#ifndef NEOPIXEL_MATRIX_HPP
#define NEOPIXEL_MATRIX_HPP

#include "led_controller.hpp"

class NeopixelMatrix {
    private:
    LedController* lc;
    uint8_t strip;
    size_t offset;
    size_t x_size;
    size_t y_size;

    public:
    NeopixelMatrix(LedController& lc, uint8_t strip, size_t offset, size_t x_size, size_t y_size) {
        this->lc = &lc;
        this->strip = strip;
        this->offset = offset;
        this->x_size = x_size;
        this->y_size = y_size;
    }

    void set_pixel_color(size_t x, size_t y, ColorRGB color) {
        lc->set_pixel_color(strip, offset + y * x_size + (y % 2 == 0 ? x : x_size - 1 - x), color);
    }
};

#endif
