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
 * Purpose: This module is a demo showing moving text on LED matrices
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include "driver/led_controller.h"
#include "driver/led_matrix.h"
#include "../font8x8/font8x8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define SLOW_DOWN 5

int main(int argc, char** argv) {
    if (argc < 6) {
        printf("Usage:\n");
        printf("%s path_to_device text_to_show r g b\n", argv[0]);
        return 0;
    }

    char* text = argv[2];
    size_t text_length = strlen(text);

    led_controller_t lc = {
        argv[1], // char* device_path
        5,       // size_t channel_n
        256,     // size_t led_n
        false,   // bool do_gamma_correction
        NULL,    // led_controller_channel_t* channels
    };
    led_controller_init(&lc);

    led_matrix_t matrices[] = {
        {&lc, 0, 0, 8, 32},
        {&lc, 1, 0, 8, 32},
        {&lc, 2, 0, 8, 32},
        {&lc, 3, 0, 8, 32},
        {&lc, 4, 0, 8, 8},
        {&lc, 4, 8*8, 8, 8},
    };

    uint32_t color = (atoi(argv[3]) << 16) | ((atoi(argv[4]) << 8)) | atoi(argv[5]);

    size_t pos = 0;
    size_t matrix = 0;
    size_t pos_time = 0;

    for(;;) {
        pos_time++;
        if (pos_time >= SLOW_DOWN) {
            for (size_t i = 0; i < ARRAY_SIZE(matrices); i++) {
                for (size_t j = 0; j < matrices[i].x_size; j++) {
                    for (size_t k = 0; k < matrices[i].y_size; k++) {
                        led_matrix_set_pixel_color(&(matrices[i]), j, k, 0);
                    }
                }
            }

            size_t curr_pos = pos;
            size_t curr_matrix = matrix;

            for (size_t i = text_length; i > 0; i--) {
                uint8_t character = text[i-1];

                for (size_t j = 0; j < 8; j++) {
                    uint8_t mask = 1 << (7-j);
                    for (size_t k = 0; k < 8; k++) {
                        led_matrix_set_pixel_color(&(matrices[curr_matrix]), k, curr_pos, font8x8_basic[character < 128 ? character : '?'][7-k] & mask ? color : 0);
                    }

                    curr_pos++;
                    if (curr_pos >= matrices[curr_matrix].y_size) {
                        curr_pos = 0;
                        curr_matrix++;
                        if (curr_matrix >= ARRAY_SIZE(matrices)) {
                            curr_matrix = 0;
                        }
                    }
                }
            }

            pos++;
            if (pos >= matrices[matrix].y_size) {
                pos = 0;
                matrix++;
                if (matrix >= ARRAY_SIZE(matrices)) {
                    matrix = 0;
                }
            }

            pos_time = 0;
        }

        led_controller_render(&lc);
    }

    led_controller_cleanup(&lc);

    return 0;
}
