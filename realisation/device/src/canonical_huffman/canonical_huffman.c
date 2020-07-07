
#include "canonical_huffman.h"

inline static bool get_bit(volatile uint8_t* buffer, uint32_t bit_index) {
    return buffer[bit_index / 8] & (1 << (bit_index % 8));
}

void canonical_huffman_decode(volatile uint8_t* size_counts, volatile uint8_t* sorted_symbols, volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t decoded_size) {
    uint32_t encoded_index = 0;
    uint32_t decoded_index = 0;

    if (size_counts[0]) {
        while (decoded_index < decoded_size) {
            decoded_data[decoded_index++] = sorted_symbols[encoded_data[encoded_index++]];
        }
        return;
    }

    while (decoded_index < decoded_size) {
        uint32_t row = 1;

        uint32_t first_index_on_row = 0;
        uint32_t first_code_on_row = 0;
        uint32_t accumulator = 0;

        for (;;) {
            accumulator = (accumulator << 1) | get_bit(encoded_data, encoded_index);
            encoded_index += 1;
            first_code_on_row <<= 1;

            if (accumulator - first_code_on_row < size_counts[row]) {
                decoded_data[decoded_index] = sorted_symbols[accumulator - first_code_on_row + first_index_on_row];
                decoded_index++;
                break;
            }
            
            first_code_on_row += size_counts[row];
            first_index_on_row += size_counts[row];

            row += 1;
        }
    }
}
