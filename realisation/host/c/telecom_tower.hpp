
#ifndef TELECOM_TOWER_HPP
#define TELECOM_TOWER_HPP

#include "canonical_huffman.hpp"

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <algorithm>

#define N_SYMBOLS 1 << 8

class TelecomTower {
    private:
    std::string path;
    uint8_t encoded_buffer[512-5];
    std::vector<std::tuple<std::bitset<N_SYMBOLS>, size_t>> canonical_huffman_code;

    public:
    TelecomTower(std::string path) {
        this->path = path;

        uint8_t data[N_SYMBOLS];
        for (size_t i = 0; i < sizeof(data); i++) {
            data[i] = i;
        }
        update_huffman_code(data, sizeof(data));
    }

    void update_huffman_code(uint8_t* data, size_t data_size) {
        std::vector<size_t> huffman_code_sizes = CanonicalHuffman::get_huffman_code_sizes(data, data_size, N_SYMBOLS);
        uint8_t size_counts[N_SYMBOLS];
        uint8_t sorted_symbols[N_SYMBOLS];
        std::memset(size_counts, 0, sizeof(size_counts));
        std::memset(sorted_symbols, 0, sizeof(sorted_symbols));
        canonical_huffman_code = CanonicalHuffman::get_canonical_huffman_code(huffman_code_sizes, size_counts, sorted_symbols);

        FILE* coding_file = fopen((path + "/coding").c_str(), "rb+");
        if (coding_file == NULL) {
            throw "Failed to open file " + path + "/coding";
        }
        fwrite(size_counts, 1, sizeof(size_counts), coding_file);
        fwrite(sorted_symbols, 1, sizeof(sorted_symbols), coding_file);
        fflush(coding_file);
        fsync(fileno(coding_file));
        fclose(coding_file);
    }

    void send(uint8_t* data, size_t data_size) {
        size_t offset = 0;
        while (offset < data_size) {
            std::memset(encoded_buffer, 0, sizeof(encoded_buffer));
            size_t encoded_size;
            if (canonical_huffman_code.size()) {
                encoded_size = CanonicalHuffman::encode_data(canonical_huffman_code, data, data_size, offset, encoded_buffer, sizeof(encoded_buffer));
            } else {
                encoded_size = std::min(sizeof(encoded_buffer), data_size - offset);
                std::memcpy(encoded_buffer, data + offset, encoded_size);
            }

            FILE* data_file = fopen((path + "/data").c_str(), "rb+");
            if (data_file == NULL) {
                throw "Failed to open file " + path + "/data";
            }
            uint8_t meta[5];
            *((uint16_t*)meta) = offset;
            *((uint16_t*)(meta + 2)) = encoded_size;
            meta[4] = offset + encoded_size < data_size ? 0 : 1;
            fwrite(meta, 1, sizeof(meta), data_file);
            fwrite(encoded_buffer, 1, sizeof(encoded_buffer), data_file);
            fflush(data_file);
            fsync(fileno(data_file));
            fclose(data_file);

            offset += encoded_size;
        }
    }
};

#endif
