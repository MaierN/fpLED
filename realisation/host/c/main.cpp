
#include "canonical_huffman.hpp"
#include "telecom_tower.hpp"

#include <chrono>
#include <cstring>

int main() {

    /*CanonicalHuffman ch;

    uint8_t data[8] = {0, 1, 1, 1, 1, 2, 0, 3};

    std::vector<size_t> test = ch.get_huffman_code_sizes(data, 8, 256);

    std::cout << "huffman_code:" << std::endl;
    for (size_t i : test) {
        std::cout << i << " ";
    }
    std::cout << std::endl << std::endl;

    std::tuple<std::vector<std::tuple<std::bitset<256>, size_t>>, std::vector<uint8_t>, std::vector<uint8_t>> can = ch.get_canonical_huffman_code(test);

    std::cout << "size_counts:" << std::endl;
    for (uint8_t size_count : std::get<1>(can)) {
        std::cout << size_count << " ";
    }
    std::cout << std::endl << std::endl;

    std::cout << "sorted_symbols:" << std::endl;
    for (uint8_t symbol : std::get<2>(can)) {
        std::cout << (int)symbol << " ";
    }
    std::cout << std::endl << std::endl;*/

    TelecomTower tt("../../test");
    uint8_t buffer[256*3*8];
    std::memset(buffer, 0, sizeof(buffer));

    /*buffer[1] = 255;
    buffer[5] = 255;
    tt.send(buffer, sizeof(buffer));

    if (true) return 0;*/

    size_t test = 0;
    size_t pos = 0;
    size_t count = 0;
    size_t max_count = 1;
    std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
    for(;;) {
        if (test % 1 == 0) {
            buffer[pos] = 0x00;
            pos = (pos+1) % sizeof(buffer);
            buffer[pos] = 0xff;
        }

        if (test % 2 == 0) buffer[10] = 0x00;
        else buffer[10] = 0xff;
        test += 1;

        tt.send(buffer, sizeof(buffer));
        //time.sleep(0.2)

        count += 1;

        if (count % max_count == 0) {
            std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
            double fps = max_count/(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()/1000000.0) * 10;
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
