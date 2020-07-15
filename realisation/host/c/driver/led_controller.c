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
 * Project: HEIA-FR / Fast ws281x LED control in parallel via USB
 *
 * Purpose: This module is a driver for the USB LED control device
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include "led_controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LED_BYTE_N 3

static const uint8_t LED_CONTROLLER_GAMMA_CORRECTION[] = {
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

/**
 * Writes the given data to the given file, and uses fsync to ensure the device has received the changes
 */
static void write_to_file(led_controller_t* lc, char* file_name, uint8_t* data, size_t data_size) {
    char* path = malloc(strlen(lc->device_path) + strlen(file_name) + 1);
    memset(path, 0x00, strlen(lc->device_path) + strlen(file_name) + 1);
    strcat(path, lc->device_path);
    strcat(path, file_name);

    FILE* file = fopen(path, "rb+");
    if (file == NULL) {
        printf("Failed to open file %s\n", path);
        exit(1);
    }
    fwrite(data, 1, data_size, file);
    fflush(file);
    fsync(fileno(file));
    fclose(file);

    free(path);
}

void led_controller_init(led_controller_t* lc) {
    // allocates data buffer
    lc->channels = malloc(sizeof(led_controller_channel_t) * lc->channel_n);
    memset(lc->channels, 0x00, sizeof(led_controller_channel_t) * lc->channel_n);

    for (size_t i = 0; i < lc->channel_n; i++) {
        lc->channels[i].leds = malloc(sizeof(led_controller_led_t) * lc->led_n);
        memset(lc->channels[i].leds, 0x00, sizeof(led_controller_led_t) * lc->led_n);
    }

    // sends config to device
    uint8_t config_buffer[4] = {0};
    config_buffer[0] = 0;
    config_buffer[1] = lc->channel_n;
    *((uint16_t*)(config_buffer + 2)) = lc->led_n;
    write_to_file(lc, "/config", config_buffer, sizeof(config_buffer));

    // sends coding  to device: no huffman code
    uint8_t coding_buffer[1] = {1};
    write_to_file(lc, "/coding", coding_buffer, sizeof(coding_buffer));
}

void led_controller_cleanup(led_controller_t* lc) {
    // frees allocated memory
    for (size_t i = 0; i < lc->channel_n; i++) {
        free(lc->channels[i].leds);
    }
    free(lc->channels);
}

void led_controller_render(led_controller_t* lc) {
    size_t curr_channel = 0;
    size_t curr_led = 0;
    size_t curr_byte = 0;
    size_t offset = 0;
    
    uint8_t data_send_buffer[512];
    uint8_t* encoded_buffer = data_send_buffer + 5;
    size_t encoded_buffer_size = sizeof(data_send_buffer) - 5;

    size_t data_size = lc->led_n * lc->channel_n * LED_BYTE_N;

    while (offset < data_size) {
        size_t encoded_size = 0;
        while (encoded_size < encoded_buffer_size) {
            led_controller_led_t led = lc->channels[curr_channel].leds[curr_led];
            uint8_t byte;

            if (curr_byte == 0) byte = (led & 0x00ff00) >> 8;
            else if (curr_byte == 1) byte = (led & 0xff0000) >> 16;
            else byte = (led & 0x0000ff) >> 0;

            if (lc->do_gamma_correction) {
                byte = LED_CONTROLLER_GAMMA_CORRECTION[byte];
            }

            encoded_buffer[encoded_size++] = byte;

            curr_channel++;
            if (curr_channel >= lc->channel_n) {
                curr_channel = 0;
                curr_byte++;
                if (curr_byte >= LED_BYTE_N) {
                    curr_byte = 0;
                    curr_led++;
                    if (curr_led >= lc->led_n) {
                        break;
                    }
                }
            }
        }

        *((uint16_t*)data_send_buffer) = offset;
        *((uint16_t*)(data_send_buffer + 2)) = encoded_size;
        data_send_buffer[4] = offset + encoded_size < data_size ? 0 : 1;
        
        write_to_file(lc, "/data", data_send_buffer, sizeof(data_send_buffer));

        offset += encoded_size;
    }
}
