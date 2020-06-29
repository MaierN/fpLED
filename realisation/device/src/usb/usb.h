
#ifndef USB_H
#define USB_H

#include <stdint.h>

#define STORAGE_LUN_NBR 1
#define STORAGE_BLK_NBR (4+1+12)
#define STORAGE_BLK_SIZ 512

extern volatile uint8_t usb_bit_buffer[];
void usb_init();
void usb_read(uint8_t *buffer, uint32_t block_address, uint16_t block_count);
void usb_write(uint8_t *buffer, uint32_t block_address, uint16_t block_count);

#endif
