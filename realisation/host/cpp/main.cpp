
#include "driver/led_controller.hpp"
#include "driver/neopixel_matrix.hpp"

#include <chrono>
#include <cstring>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " path_to_device" << std::endl;
        return 0;
    }

    LedController lc(argv[1], true, 0, 5, 256);

    uint8_t test_data[4096];
    for (size_t i = 0; i < 256; i++) {
        test_data[i] = i;
    }
    lc.update_huffman_code(test_data, sizeof(test_data));

    NeopixelMatrix matrix_0(lc, 4, 64, 8, 8);

    size_t test = 0;
    size_t pos = 0;
    size_t count = 0;
    size_t max_count = 10;

    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    for(;;) {
        if (test % 10 == 0) {
            for (size_t i = 0; i < 8; i++) {
                matrix_0.set_pixel_color(i, pos%8, {0, 0, 0});
            }

            lc.debug_set_byte(pos, 0x00);
            pos = (pos+1) % (3*8*256);
            lc.debug_set_byte(pos, 0xff);

            for (size_t i = 0; i < 8; i++) {
                matrix_0.set_pixel_color(i, pos%8, {0, 60, 60});
            }
        }

        lc.debug_set_byte(0, test % 2 == 0 ? 0x00 : 0xff);
        test += 1;

        //lc.update_huffman_code(buffer, sizeof(buffer));
        lc.render();
        //time.sleep(0.2)

        count += 1;

        if (count % max_count == 0) {
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            double fps = max_count/(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()/1000000.0);
            if (fps < 10) {
                std::cout << "----------------" << std::endl;
                std::cout << "----------------" << std::endl;
            }
            std::cout << "fps: " << fps << std::endl;
            t0 = t1;
        }
    }

    return 0;
}
