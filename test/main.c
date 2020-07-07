
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static bool get_bit(volatile uint8_t* buffer, uint32_t bit_index) {
    if (bit_index < 2) return bit_index;
    bit_index -= 2;
    return buffer[bit_index / 8] & (1 << (bit_index % 8));
}

static void set_bit(volatile uint8_t* buffer, uint32_t bit_index, bool set) {
    if (set) buffer[bit_index / 8] |= 1 << (bit_index % 8);
    else buffer[bit_index / 8] &= ~(1 << (bit_index % 8));
}

void lz_decode(volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t length) {
    uint32_t table[64] = {0};

    uint8_t table_index = 0;
    uint32_t encoded_bit_index = 0;
    uint32_t decoded_bit_index = 0;
    uint8_t current_variable_length = 2;

    table[table_index] = 0 | (1 << 24);
    table_index++;
    table[table_index] = 1 | (1 << 24);
    table_index++;

    while (decoded_bit_index < length) {
        // get index
        uint8_t current_index = 0;
        for (size_t i = 0; i < current_variable_length; i++) {
            current_index <<= 1;
            current_index |= get_bit(encoded_data, encoded_bit_index+2) ? 1 : 0;
            encoded_bit_index++;
        }

        // get current prefix
        uint32_t prefix_current_entry_offset = table[current_index] & 0x00ffffff;
        uint32_t prefix_current_entry_length = table[current_index] >> 24;

        // prepare curent entry in table
        table[table_index] = ((decoded_bit_index+2) & 0x00ffffff) | ((prefix_current_entry_length+1) << 24);
        table_index++;

        // change variable length
        if (table_index >= 1+(1 << current_variable_length)) {
            current_variable_length++;
        }

        // add prefix to decoded data
        for (size_t i = 0; i < prefix_current_entry_length; i++) {
            set_bit(decoded_data, decoded_bit_index, get_bit(decoded_data, prefix_current_entry_offset + i));
            decoded_bit_index++;
        }
    }
}

void print_buffer(volatile uint8_t* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < 8; j++) {
            printf("%u ", (buffer[i] >> j ) & 1);
        }
        printf("\n");
    }
}

volatile uint8_t encoded_data_a[8] = {0};
volatile uint8_t decoded_data_a[8] = {0};

int main() {
    /*
    1 0 1 10 01 1

    0: 0
    1: 1
    10: 10
    11: 01
    100: 11
    101: 100
    110: 011
    */
    uint32_t index = 0;

set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 1);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);
set_bit(encoded_data_a, index++, 0);

    lz_decode(encoded_data_a, decoded_data_a, 32);

    printf("------\n");
    print_buffer(encoded_data_a, 8);
    printf("------\n");
    print_buffer(decoded_data_a, 8);

    return 0;
}
