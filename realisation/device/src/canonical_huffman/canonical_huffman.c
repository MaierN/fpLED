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
 * Purpose: This module handles canonical huffman decoding (https://en.wikipedia.org/wiki/Canonical_Huffman_code)
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include <string.h>

#include "canonical_huffman.h"

// each individual bit in the RAM is mapped to an uint32_t at a virtual address, which can be used to do operations on bits very fast (1 instruction to read or write), this is called "bit banding"
#define BITBANDING_BASE_ADDRESS 0x20000000
#define BITBANDING_BITBAND_ADDRESS 0x22000000
#define BITBANDING_GET_ADDRESS(var_address, bit_offset) ((volatile uint32_t *) (BITBANDING_BITBAND_ADDRESS + ((uint32_t)(var_address - BITBANDING_BASE_ADDRESS) << 5) + (bit_offset << 2)))

void canonical_huffman_decode(volatile uint8_t* size_counts, volatile uint8_t* sorted_symbols, volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t decoded_size) {
    uint32_t decoded_index = 0;

    if (size_counts[0]) {
        // special case : no huffman coding used (simply copy the data)
        memcpy((void*)decoded_data, (void*)encoded_data, decoded_size);
        return;
    }

    // canonical huffman decoding, as described here:
    // -> https://en.wikipedia.org/wiki/Canonical_Huffman_code
    // -> http://pinop8.webfactional.com/mw/index.php?title=Canonical_Huffman_code

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
