
#include "driver/led_controller.hpp"
#include "driver/neopixel_matrix.hpp"

#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " path_to_device" << std::endl;
        return 0;
    }

    LedController lc(argv[1], true, 0, 5, 256);

    std::vector<NeopixelMatrix> matrices;
    matrices.push_back(NeopixelMatrix(lc, 0, 0, 8, 32));
    matrices.push_back(NeopixelMatrix(lc, 1, 0, 8, 32));
    matrices.push_back(NeopixelMatrix(lc, 2, 0, 8, 32));
    matrices.push_back(NeopixelMatrix(lc, 3, 0, 8, 32));
    matrices.push_back(NeopixelMatrix(lc, 4, 0, 8, 8));
    matrices.push_back(NeopixelMatrix(lc, 4, 8*8, 8, 8));

    size_t pos_time = 0;
    size_t pos = 0;
    size_t count = 0;
    size_t max_count = 100;
    size_t curr_matrix = 0;
    ColorRGB color = {0, 60, 60};
    
    for (size_t i = 0; i < 8; i++) {
        matrices[curr_matrix].set_pixel_color(i, pos, color);
    }
    lc.update_huffman_code();

    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    for(;;) {
        pos_time++;
        if (pos_time >= 10) {
            for (size_t i = 0; i < 8; i++) {
                matrices[curr_matrix].set_pixel_color(i, pos, {0, 0, 0});
            }

            pos++;
            if (pos >= matrices[curr_matrix].y_size) {
                pos = 0;
                curr_matrix++;
                if (curr_matrix >= matrices.size()) {
                    curr_matrix = 0;
                }
            }
            
            for (size_t i = 0; i < 8; i++) {
                matrices[curr_matrix].set_pixel_color(i, pos, color);
            }

            pos_time = 0;
        }

        lc.debug_set_byte(0, count % 2 == 0 ? 0x00 : 0xff);

        lc.render();

        count += 1;
        if (count % max_count == 0) {
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            double fps = max_count/(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()/1000000.0);
            if (fps < 10) {
                std::cout << "----------------" << std::endl;
            }
            std::cout << "fps: " << fps << std::endl;
            t0 = t1;
        }
    }

    return 0;
}
