
#include "canonical_huffman.hpp"
#include "led_controller.hpp"

#include <chrono>
#include <cstring>

int main() {

    LedController lc("../../test", 1, 8, 256);

    uint8_t test_data[1024];
    for (size_t i = 0; i < 256; i++) {
        test_data[i] = i;
    }
    lc.update_huffman_code(test_data, sizeof(test_data));

    //lc.send();

    size_t test = 0;
    size_t pos = 0;
    size_t count = 0;
    size_t max_count = 10;

    for (size_t i = 0; i < 2 * 32 * 3; i+=3) {
        lc.debug_set_byte(i, 0x01);
    }

    /*for (;;) {
        if (test % 5 == 0) {
            buffer[25] = pos;
            buffer[420] = pos;
            printf("%i\n", pos);
            pos++;
            pos %= 256;
        }
        test++;
        lc.send(buffer, sizeof(buffer));
    }*/
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    for(;;) {
        if (test % 50 == 0) {
            lc.debug_set_byte(pos, 0x00);
            pos = (pos+1) % (3*8*256);
            lc.debug_set_byte(pos, 0xff);
        }

        lc.debug_set_byte(10, test % 2 == 0 ? 0x00 : 0xff);
        test += 1;

        //lc.update_huffman_code(buffer, sizeof(buffer));
        lc.send();
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
