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
 * Project: HEIA-FR / Fast ws281x LED control in parallel via USB
 *
 * Purpose: This module is a demo showing moving text on LED matrices
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include "driver/led_controller.hpp"
#include "driver/led_matrix.hpp"
#include "../font8x8/font8x8.h"

#include <iostream>
#include <string>
#include <cstring>

#define SLOW_DOWN 5

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " path_to_device text_to_show r g b" << std::endl;
        return 0;
    }

    char* text = argv[2];
    size_t text_length = strlen(text);

    LedController lc(argv[1], false, 0, 5, 256);

    std::vector<LedMatrix> matrices;
    matrices.push_back(LedMatrix(lc, 0, 0, 8, 32));
    matrices.push_back(LedMatrix(lc, 1, 0, 8, 32));
    matrices.push_back(LedMatrix(lc, 2, 0, 8, 32));
    matrices.push_back(LedMatrix(lc, 3, 0, 8, 32));
    matrices.push_back(LedMatrix(lc, 4, 0, 8, 8));
    matrices.push_back(LedMatrix(lc, 4, 8*8, 8, 8));
    
    ColorRGB color = {
        (uint8_t)std::stoi(argv[3]),
        (uint8_t)std::stoi(argv[4]),
        (uint8_t)std::stoi(argv[5]),
    };

    size_t pos = 0;
    size_t matrix = 0;
    size_t pos_time = 0;

    for(;;) {
        pos_time++;
        if (pos_time >= SLOW_DOWN) {
            for (size_t i = 0; i < matrices.size(); i++) {
                for (size_t j = 0; j < matrices[i].x_size; j++) {
                    for (size_t k = 0; k < matrices[i].y_size; k++) {
                        matrices[i].set_pixel_color(j, k, {0, 0, 0});
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
                        matrices[curr_matrix].set_pixel_color(k, curr_pos, font8x8_basic[character < 128 ? character : '?'][7-k] & mask ? color : ColorRGB {0, 0, 0});
                    }

                    curr_pos++;
                    if (curr_pos >= matrices[curr_matrix].y_size) {
                        curr_pos = 0;
                        curr_matrix++;
                        if (curr_matrix >= matrices.size()) {
                            curr_matrix = 0;
                        }
                    }
                }
            }

            pos++;
            if (pos >= matrices[matrix].y_size) {
                pos = 0;
                matrix++;
                if (matrix >= matrices.size()) {
                    matrix = 0;
                }
            }

            pos_time = 0;
        }

        lc.render();
    }

    return 0;
}
