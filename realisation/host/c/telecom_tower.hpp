
#ifndef TELECOM_TOWER_HPP
#define TELECOM_TOWER_HPP

#include "canonical_huffman.hpp"

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>

class TelecomTower {
    private:
    std::string path;
    uint8_t encoded_buffer[512-5];
    CanonicalHuffman can_huf;

    public:
    TelecomTower(std::string path) {
        this->path = path;
    }

    void send(uint8_t* data, size_t data_size) {

        std::vector<size_t> huffman_code_sizes = can_huf.get_huffman_code_sizes(data, data_size, 256);
        std::tuple<std::vector<std::tuple<std::bitset<256>, size_t>>, std::vector<uint8_t>, std::vector<uint8_t>> canonical_huffman = can_huf.get_canonical_huffman_code(huffman_code_sizes);

        std::ofstream coding_file;
        coding_file.open(path + "/coding", std::ios::out | std::ios::binary);
        for (uint8_t size_count : std::get<1>(canonical_huffman)) {
            coding_file << size_count;
        }
        for (uint8_t symbol : std::get<2>(canonical_huffman)) {
            coding_file << symbol;
        }
        coding_file.close();
        sync();

        //for (size_t test = 0; test < 10; test++) {

        size_t offset = 0;
        while (offset < data_size) {
            std::memset(encoded_buffer, 0, sizeof(encoded_buffer));
            size_t encoded_size = can_huf.encode_data(std::get<0>(canonical_huffman), data, data_size, offset, encoded_buffer, sizeof(encoded_buffer));

            std::ofstream data_file;
            data_file.open(path + "/data", std::ios::out | std::ios::binary);
            uint8_t meta[5];
            *((uint16_t*)meta) = offset;
            *((uint16_t*)(meta + 2)) = encoded_size;
            meta[4] = offset + encoded_size < data_size ? 0 : 1;
            data_file.write((char*)meta, sizeof(meta));
            data_file.write((char*)encoded_buffer, sizeof(encoded_buffer));
            data_file.close();
            sync();

            offset += encoded_size;
            
        }
            
        //}
    }
};

#endif
