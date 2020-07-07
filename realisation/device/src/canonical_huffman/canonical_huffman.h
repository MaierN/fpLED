
#ifndef CANONICAL_HUFFMAN_H
#define CANONICAL_HUFFMAN_H

#include <stdint.h>
#include <stdbool.h>

void canonical_huffman_decode(volatile uint8_t* size_counts, volatile uint8_t* sorted_symbols, volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t decoded_size);

#endif
