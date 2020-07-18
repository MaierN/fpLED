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
 * Purpose: This module is a library for the USB LED control device
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#ifndef TELECOM_TOWER_HPP
#define TELECOM_TOWER_HPP

#include "canonical_huffman.hpp"

#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <algorithm>

#ifdef __linux__ 
    #include <unistd.h>
#elif _WIN32
    #undef UNICODE
    #include <windows.h>
#endif

#define N_SYMBOLS (1 << 8)
#define MAX_STRIP_N 8
#define MAX_LEDS_BUFFER_SIZE (3*8*512)
#define LED_BYTE_N 3
#define DMA_BUFFER_LED_N 4
#define BLOCK_SIZE 512
#define TRANSFER_PARAMETERS_SIZE 5

// LED color (RGB)
struct ColorRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

// Gamma correction matrix, can be used to get more accurate colors
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

    uint8_t data_send_buffer[BLOCK_SIZE]; // buffer containing transfer parameters and encoded data
    uint8_t* encoded_buffer = data_send_buffer + TRANSFER_PARAMETERS_SIZE; // part of data_send_buffer containing the encoded data
    size_t encoded_buffer_size = sizeof(data_send_buffer) - TRANSFER_PARAMETERS_SIZE;

    std::vector<std::tuple<std::bitset<N_SYMBOLS>, size_t>> canonical_huffman_code;

    /**
     * Writes the given data to the given file, and uses fsync to ensure the device has received the changes
     */
    void write_to_file(std::string path, uint8_t* data, size_t data_size) {
        #ifdef __linux__
            FILE* file = fopen(path.c_str(), "rb+");
            if (file == NULL) {
                throw "Failed to open file " + path;
            }
            fwrite(data, 1, data_size, file);
            fflush(file);
            fsync(fileno(file));
            fclose(file);
        #elif _WIN32
            HANDLE handle = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            DWORD count;
            WriteFile(handle, data, data_size, &count, NULL);
            FlushFileBuffers(handle);
            CloseHandle(handle);
        #endif
    }

    /**
     * Sets a byte in the data buffer, handling the reduction mode
     */
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

        // allocates data buffer
        data_buffer_size = strip_n * led_n * LED_BYTE_N / (reduction_mode ? 2 : 1);
        data_buffer = new uint8_t[data_buffer_size];
        std::memset(data_buffer, 0x00, data_buffer_size);

        // sends config to device
        uint8_t config_buffer[4] = {0};
        config_buffer[0] = reduction_mode;
        config_buffer[1] = strip_n;
        *((uint16_t*)(config_buffer + 2)) = led_n;
        write_to_file(path + "/config", config_buffer, sizeof(config_buffer));

        // sends coding to device: no huffman code
        uint8_t data[N_SYMBOLS];
        for (size_t i = 0; i < sizeof(data); i++) {
            data[i] = i;
        }
        update_huffman_code(data, sizeof(data));
    }

    ~LedController() {
        delete[] data_buffer;
    }

    /**
     * Sets the color of a given pixel on a given strip
     */
    void set_pixel_color(uint8_t strip, size_t pixel, ColorRGB color) {
        size_t index = strip_n * pixel * LED_BYTE_N + strip;

        set_byte_value(index, color.g);
        set_byte_value(index + strip_n, color.r);
        set_byte_value(index + 2*strip_n, color.b);
    }

    /**
     * Updates the huffman code using the data currently in data_buffer
     */
    void update_huffman_code() {
        update_huffman_code(data_buffer, data_buffer_size);
    }

    /**
     * Updates the huffman code using the given data
     */
    void update_huffman_code(uint8_t* data, size_t data_size) {
        std::vector<size_t> huffman_code_sizes = CanonicalHuffman::get_huffman_code_sizes(data, data_size, N_SYMBOLS);
        uint8_t coding_buffer[N_SYMBOLS * 2];
        std::memset(coding_buffer, 0, sizeof(coding_buffer));

        canonical_huffman_code = CanonicalHuffman::get_canonical_huffman_code(huffman_code_sizes, coding_buffer, coding_buffer + N_SYMBOLS);
        write_to_file(path + "/coding", coding_buffer, sizeof(coding_buffer));
    }

    /**
     * Sends the prepared data to the device, encoded with the prepared huffman code
     */
    size_t render() {
        size_t total_sent_size = 0;
        size_t offset = 0;
        while (offset < data_buffer_size) {
            size_t encoded_size;
            if (canonical_huffman_code.size()) {
                encoded_size = CanonicalHuffman::encode_data(canonical_huffman_code, data_buffer, data_buffer_size, offset, encoded_buffer, encoded_buffer_size);
            } else {
                encoded_size = (std::min)(encoded_buffer_size, data_buffer_size - offset);
                std::memcpy(encoded_buffer, data_buffer + offset, encoded_size);
            }

            *((uint16_t*)data_send_buffer) = offset;
            *((uint16_t*)(data_send_buffer + 2)) = encoded_size;
            data_send_buffer[4] = offset + encoded_size < data_buffer_size ? 0 : 1;

            write_to_file(path + "/data", data_send_buffer, sizeof(data_send_buffer));
            
            total_sent_size += sizeof(data_send_buffer);

            offset += encoded_size;
        }

        return total_sent_size;
    }
};

#endif
