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
 * Purpose: This module is a demo which can be used to test this project with various parameters
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include "lib/led_controller.hpp"

#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char** argv) {
    if (argc < 11) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " path_to_device strip_n led_n do_gamma_correction use_data_reduction use_huffman_coding animation r g b" << std::endl;
        return 0;
    }

    size_t param_i = 0;
    std::cout << "Running test with the following parameters:" << std::endl;
    std::cout << "         path_to_device: " << argv[++param_i] << std::endl;
    std::cout << "                strip_n: " << argv[++param_i] << std::endl;
    std::cout << "                  led_n: " << argv[++param_i] << std::endl;
    std::cout << "    do_gamma_correction: " << argv[++param_i] << std::endl;
    std::cout << "     use_data_reduction: " << argv[++param_i] << std::endl;
    std::cout << "     use_huffman_coding: " << argv[++param_i] << std::endl;
    std::cout << "              animation: " << argv[++param_i] << std::endl;
    std::cout << "                      r: " << argv[++param_i] << std::endl;
    std::cout << "                      g: " << argv[++param_i] << std::endl;
    std::cout << "                      b: " << argv[++param_i] << std::endl;
    std::cout << "(total: " << param_i << " parameters)" << std::endl << std::endl;

    size_t strip_n = std::stoi(argv[2]);
    size_t led_n = std::stoi(argv[3]);
    bool use_huffman_coding = std::stoi(argv[6]);
    bool animation = std::stoi(argv[7]);

    LedController lc(argv[1], std::stoi(argv[4]), std::stoi(argv[5]), strip_n, led_n);

    ColorRGB color = {
        (uint8_t)std::stoi(argv[8]),
        (uint8_t)std::stoi(argv[9]),
        (uint8_t)std::stoi(argv[10]),
    };

    if (!animation) {
        for (size_t i = 0; i < strip_n; i++) {
            for (size_t j = 0; j < led_n; j++) {
                lc.set_pixel_color(i, j, color);
            }
        }
        if (use_huffman_coding) {
            lc.update_huffman_code();
        }
        lc.render();
    } else {

        size_t curr_strip = 0;
        size_t pos = 0;

        lc.set_pixel_color(curr_strip, pos, color);
        if (use_huffman_coding) {
            lc.update_huffman_code();
        }

        size_t count = 0;
        size_t max_count = 64;
        std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
        for(;;) {
            lc.set_pixel_color(curr_strip, pos, {0, 0, 0});

            pos++;
            if (pos >= led_n) {
                pos = 0;
                curr_strip++;
                if (curr_strip >= strip_n) {
                    curr_strip = 0;
                }
            }
            
            lc.set_pixel_color(curr_strip, pos, color);

            lc.render();

            count++;
            if (count % max_count == 0) {
                std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
                double fps = max_count/(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()/1000000.0);
                std::cout << "fps: " << fps << std::endl;
                t0 = t1;
            }
        }
    }

    return 0;
}

