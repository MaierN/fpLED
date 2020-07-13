
#include <string.h>

#include "canonical_huffman.h"

#define BITBANDING_BASE_ADDRESS 0x20000000
#define BITBANDING_BITBAND_ADDRESS 0x22000000
#define BITBANDING_GET_ADDRESS(var_address, bit_offset) ((volatile uint32_t *) (BITBANDING_BITBAND_ADDRESS + ((uint32_t)(var_address - BITBANDING_BASE_ADDRESS) << 5) + (bit_offset << 2)))

inline static bool get_bit(volatile uint8_t* buffer, uint32_t bit_index) {
    return buffer[bit_index / 8] & (1 << (bit_index % 8));
}

void canonical_huffman_decode(volatile uint8_t* size_counts, volatile uint8_t* sorted_symbols, volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t decoded_size) {
    //uint32_t encoded_index = 0;
    uint32_t decoded_index = 0;

    if (size_counts[0]) {
        /*while (decoded_index < decoded_size) {
            decoded_data[decoded_index++] = encoded_data[encoded_index++];//sorted_symbols[encoded_data[encoded_index++]];
        }*/
        memcpy((void*)decoded_data, (void*)encoded_data, decoded_size);
        return;
    }

    volatile uint32_t* bitband_addr = BITBANDING_GET_ADDRESS(encoded_data, 0);

    while (decoded_index < decoded_size) {
        uint32_t row = 1;

        uint32_t first_index_on_row = 0;
        uint32_t first_code_on_row = 0;
        uint32_t accumulator = 0;

        for (;;) {
            accumulator = (accumulator << 1) | (*bitband_addr);
            bitband_addr += 1;
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
