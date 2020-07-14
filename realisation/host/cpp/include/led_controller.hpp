
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
#define MAX_STRIP_N 8
#define MAX_LEDS_BUFFER_SIZE (3*8*512)
#define LED_BYTE_N 3
#define DMA_BUFFER_LED_N 4

struct ColorRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// source: https://learn.adafruit.com/led-tricks-gamma-correction/the-quick-fix
const uint8_t LED_CONTROLLER_GAMMA_CORRECTION[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

class LedController {
    private:
    std::string path;
    bool do_gamma_correction;
    uint8_t reduction_mode;
    uint8_t strip_n;
    uint16_t led_n;

    uint8_t* data_buffer;
    size_t data_buffer_size;

    uint8_t encoded_buffer[512-5];
    std::vector<std::tuple<std::bitset<N_SYMBOLS>, size_t>> canonical_huffman_code;


    public:
    LedController(std::string path, bool do_gamma_correction, uint8_t reduction_mode, uint8_t strip_n, uint16_t led_n) {
        if (reduction_mode > 1) {
            throw "Invalid reduction mode";
        }
        if (strip_n > MAX_STRIP_N) {
            throw "Invalid strip_n";
        }
        if (led_n * strip_n * LED_BYTE_N / (reduction_mode ? 2 : 1) > MAX_LEDS_BUFFER_SIZE) {
            throw "Invalid led_n (too many LEDs for this strip_n)";
        }
        if (led_n % DMA_BUFFER_LED_N != 0) {
            throw "Invalid led_n (must be divisible by DMA_BUFFER_LED_N)";
        }

        this->path = path;
        this->do_gamma_correction = do_gamma_correction;
        this->reduction_mode = reduction_mode;
        this->strip_n = strip_n;
        this->led_n = led_n;

        data_buffer_size = strip_n * led_n * LED_BYTE_N / (reduction_mode ? 2 : 1);
        data_buffer = new uint8_t[data_buffer_size];
        std::memset(data_buffer, 0x00, data_buffer_size);

        uint8_t config_buffer[4] = {0};
        config_buffer[0] = reduction_mode;
        config_buffer[1] = strip_n;
        *((uint16_t*)(config_buffer + 2)) = led_n;

        FILE* config_file = fopen((path + "/config").c_str(), "rb+");
        if (config_file == NULL) {
            throw "Failed to open file " + path + "/config";
        }
        fwrite(config_buffer, 1, sizeof(config_buffer), config_file);
        fflush(config_file);
        fsync(fileno(config_file));
        fclose(config_file);

        uint8_t data[N_SYMBOLS];
        for (size_t i = 0; i < sizeof(data); i++) {
            data[i] = i;
        }
        update_huffman_code(data, sizeof(data));
    }

    ~LedController() {
        delete[] data_buffer;
    }

    void set_byte_value(size_t index, uint8_t value) {
        if (do_gamma_correction) {
            value = LED_CONTROLLER_GAMMA_CORRECTION[value];
        }

        if (reduction_mode) {
            size_t i = index/2;
            uint8_t val = value & 0xf0;
            if (index % 2 == 0) {
                data_buffer[i] &= 0x0f;
                data_buffer[i] |= val;
            } else {
                data_buffer[i] &= 0xf0;
                data_buffer[i] |= val >> 4;
            }
        } else {
            data_buffer[index] = value;
        }
    }

    void set_pixel_color(uint8_t strip, size_t pixel, ColorRGB color) {
        size_t index = strip_n * pixel * LED_BYTE_N + strip;

        set_byte_value(index, color.g);
        set_byte_value(index + strip_n, color.r);
        set_byte_value(index + 2*strip_n, color.b);
    }

    void debug_set_byte(size_t index, uint8_t value) {
        if (index < data_buffer_size) {
            data_buffer[index] = value;
        }
    }

    void update_huffman_code() {
        update_huffman_code(data_buffer, data_buffer_size);
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

    void send() {
        size_t offset = 0;
        while (offset < data_buffer_size) {
            std::memset(encoded_buffer, 0, sizeof(encoded_buffer));
            size_t encoded_size;
            if (canonical_huffman_code.size()) {
                encoded_size = CanonicalHuffman::encode_data(canonical_huffman_code, data_buffer, data_buffer_size, offset, encoded_buffer, sizeof(encoded_buffer));
            } else {
                encoded_size = std::min(sizeof(encoded_buffer), data_buffer_size - offset);
                std::memcpy(encoded_buffer, data_buffer + offset, encoded_size);
            }

            FILE* data_file = fopen((path + "/data").c_str(), "rb+");
            if (data_file == NULL) {
                throw "Failed to open file " + path + "/data";
            }
            uint8_t meta[5];
            *((uint16_t*)meta) = offset;
            *((uint16_t*)(meta + 2)) = encoded_size;
            meta[4] = offset + encoded_size < data_buffer_size ? 0 : 1;
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
