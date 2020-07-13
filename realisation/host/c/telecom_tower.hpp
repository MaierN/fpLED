
#ifndef TELECOM_TOWER_HPP
#define TELECOM_TOWER_HPP

#include "canonical_huffman.hpp"

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

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

        uint8_t test_data[256];
        for (size_t i = 0; i < 256; i++) {
            test_data[i] = i;
        }

        std::vector<size_t> huffman_code_sizes = can_huf.get_huffman_code_sizes(test_data, 256, 256);
        uint8_t size_counts[256];
        uint8_t sorted_symbols[256];
        std::memset(size_counts, 0, sizeof(size_counts));
        std::memset(sorted_symbols, 0, sizeof(sorted_symbols));
        std::vector<std::tuple<std::bitset<256>, size_t>> canonical_huffman_code = can_huf.get_canonical_huffman_code(huffman_code_sizes, size_counts, sorted_symbols);

        /*std::ofstream coding_file;
        coding_file.open(path + "/coding", std::ios::out | std::ios::binary);
        coding_file.write((char*)size_counts, sizeof(size_counts));
        coding_file.write((char*)sorted_symbols, sizeof(sorted_symbols));
        coding_file.close();
        sync();*/


        for (size_t test = 0; test < 10; test++) {
        size_t offset = 0;
        while (offset + 512-5 < data_size) {
            //std::memset(encoded_buffer, 0, sizeof(encoded_buffer));
            //size_t encoded_size = can_huf.encode_data(canonical_huffman_code, data, data_size, offset, encoded_buffer, sizeof(encoded_buffer));
            size_t encoded_size = 512-5;

            //std::ofstream data_file;
            //data_file.open(path + "/data", std::ios::out | std::ios::binary);
            FILE* data_file = fopen("../../test/data", "rb+");
            uint8_t meta[5];
            *((uint16_t*)meta) = offset;
            *((uint16_t*)(meta + 2)) = encoded_size;
            meta[4] = offset + encoded_size < data_size-512+5 ? 0 : 1;
            //data_file.write((char*)meta, sizeof(meta));
            //data_file.write((char*)data + offset, encoded_size);
            fwrite(meta, 1, sizeof(meta), data_file);
            fwrite(data + offset, 1, encoded_size, data_file);
            //data_file.close();
            fclose(data_file);
            sync();

            offset += encoded_size;
            
        }
            
        }
    }
};

#endif
