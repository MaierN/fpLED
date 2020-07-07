
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void lz_init();
void lz_decode_symbol(volatile uint8_t* encoded_data, volatile uint8_t* decoded_data, uint32_t* table, uint32_t* encoded_index, uint32_t* decoded_index, uint8_t* table_index);
