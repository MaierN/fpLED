
#include "canonical_huffman.hpp"
#include "telecom_tower.hpp"

#include <chrono>
#include <cstring>

int main() {

    TelecomTower tt("../../test");
    uint8_t buffer[256*3*8];
    std::memset(buffer, 0, sizeof(buffer));

    for (size_t i = 0; i < 512; i++) {
        buffer[i] = i%256;
    }
    //tt.update_huffman_code(buffer, sizeof(buffer));

    /*for (size_t i = 0; i < 5000; i++) {
        buffer[i] = i % 3 == 0 ? 0x01 : 0x1;
    }*/
    std::memset(buffer, 0, sizeof(buffer));
    //tt.send(buffer, sizeof(buffer));

    size_t test = 0;
    size_t pos = 0;
    size_t count = 0;
    size_t max_count = 10;

    /*for (;;) {
        if (test % 5 == 0) {
            buffer[25] = pos;
            buffer[420] = pos;
            printf("%i\n", pos);
            pos++;
            pos %= 256;
        }
        test++;
        tt.send(buffer, sizeof(buffer));
    }*/
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    for(;;) {
        if (test % 50 == 0) {
            buffer[pos] = 0x00;
            pos = (pos+1) % (256*3*8);
            buffer[pos] = 0xff;
        }

        /*for (size_t i = 0; i < 64; i++) {
            printf("%02x ", buffer[i]);
        }
        printf("\n");*/

        /*if (test % 2 == 0) buffer[10] = 0x00;
        else buffer[10] = 0xff;*/
        test += 1;

        //tt.update_huffman_code(buffer, sizeof(buffer));
        tt.send(buffer, sizeof(buffer));
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
