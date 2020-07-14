
#include "led_controller.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LED_BYTE_N 3

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
    lc->channels = malloc(sizeof(led_controller_channel_t) * lc->channel_n);
    memset(lc->channels, 0x00, sizeof(led_controller_channel_t) * lc->channel_n);

    for (size_t i = 0; i < lc->channel_n; i++) {
        lc->channels[i].leds = malloc(sizeof(led_controller_led_t) * lc->led_n);
        memset(lc->channels[i].leds, 0x00, sizeof(led_controller_led_t) * lc->led_n);
    }

    uint8_t config_buffer[4] = {0};
    config_buffer[0] = 0;
    config_buffer[1] = lc->channel_n;
    *((uint16_t*)(config_buffer + 2)) = lc->led_n;
    write_to_file(lc, "/config", config_buffer, sizeof(config_buffer));

    uint8_t coding_buffer[1] = {1};
    write_to_file(lc, "/coding", coding_buffer, sizeof(coding_buffer));
}

void led_controller_cleanup(led_controller_t* lc) {
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
