
#include "lz.h"

void lz_init() {

}

void lz_decode_symbol(volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t* table, uint32_t* encoded_index, uint32_t* decoded_index, uint8_t* table_index) {
    uint8_t symbol_table_index = encoded_data[*encoded_index];
    (*encoded_index)++;

    uint32_t symbol_offset = 0;
    uint8_t symbol_length = 1;
    if (symbol_table_index >= 256) {
        symbol_offset = table[symbol_table_index-256] & 0x00ffffff;
        symbol_length = table[symbol_table_index-256] >> 24;
    }

    uint8_t character = encoded_data[*encoded_index];
    (*encoded_index)++;
    
    table[*table_index] = (*decoded_index) | ((symbol_length + 1) << 24);
    (*table_index)++;

    if (symbol_table_index >= 256) {
        while (symbol_length > 0) {
            symbol_length--;

            decoded_data[*decoded_index] = decoded_data[symbol_offset];
            (*decoded_index)++;
            symbol_offset++;
        }
    } else {
        decoded_data[*decoded_index] = symbol_table_index;
        (*decoded_index)++;
    }

    decoded_data[*decoded_index] = character;
    (*decoded_index)++;
}

static bool get_bit(volatile uint8_t* buffer, uint32_t bit_index) {
    return buffer[bit_index / 8] & (1 << (bit_index % 8));
}

static bool set_bit(volatile uint8_t* buffer, uint32_t bit_index, bool set) {
    if (set) buffer[bit_index / 8] |= 1 << (bit_index % 8);
    else buffer[bit_index / 8] &= ~(1 << (bit_index % 8));
}

void lz_decode(volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t length) {
    uint32_t table[64] = {0};

    uint8_t table_index = 0;
    uint32_t encoded_bit_index = 0;
    uint32_t decoded_bit_index = 0;
    uint8_t current_variable_length = 1;

    set_bit(decoded_data, decoded_bit_index, 0);
    decoded_bit_index++;
    set_bit(decoded_data, decoded_bit_index, 1);
    decoded_bit_index++;

    table[table_index] = 0 | (1 << 24);
    table_index++;
    table[table_index] = 1 | (1 << 24);
    table_index++;

    while (decoded_bit_index < length) {
        // get index
        uint8_t current_index = 0;
        for (size_t i = 0; i < current_variable_length; i++) {
            current_index <<= 1;
            current_index |= get_bit(encoded_data, encoded_bit_index) ? 1 : 0;
            encoded_bit_index++;
        }

        // get current prefix
        uint32_t prefix_current_entry_offset = table[current_index] & 0x00ffffff;
        uint32_t prefix_current_entry_length = table[current_index] >> 24;

        // prepare curent entry in table
        table[table_index] = (decoded_bit_index & 0x00ffffff) | ((prefix_current_entry_length << 24) + 1);
        table_index++;

        // change variable length
        if (table_index >= (1 << current_variable_length)) {
            current_variable_length++;
        }

        // add prefix to decoded data
        for (size_t i = 0; i < prefix_current_entry_length; i++) {
            set_bit(decoded_data, decoded_bit_index, get_bit(encoded_data, prefix_current_entry_offset + i));
            decoded_bit_index++;
        }
    }
}
