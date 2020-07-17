/**
 * Copyright (c) 2020 University of Applied Sciences Western Switzerland / Fribourg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * Project: HEIA-FR / Fast parallel ws281x LED control via USB
 *
 * Purpose: This module can be used to control LED strips arranged in a matrix with x/y coordinates
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#ifndef LED_MATRIX_HPP
#define LED_MATRIX_HPP

#include "led_controller.hpp"

class LedMatrix {
    public:
    LedController* lc;
    uint8_t strip;
    size_t offset;
    size_t x_size;
    size_t y_size;

    LedMatrix(LedController& lc, uint8_t strip, size_t offset, size_t x_size, size_t y_size) {
        this->lc = &lc;
        this->strip = strip;
        this->offset = offset;
        this->x_size = x_size;
        this->y_size = y_size;
    }

    /**
     * Sets the color of the given pixel
     */
    void set_pixel_color(size_t x, size_t y, ColorRGB color) {
        lc->set_pixel_color(strip, offset + y * x_size + (y % 2 == 0 ? x : x_size - 1 - x), color);
    }
};

#endif
