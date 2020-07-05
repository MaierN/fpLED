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
 * Project: HEIA-FR / Accélérateur de tour télécom
 *
 * Purpose: This module handles the USB interface and simulates a mass storage device with a FAT12 filesystem
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#ifndef USB_H
#define USB_H

#include <stdint.h>

#define STORAGE_LUN_NBR 1   // Number of available Logical Unit Numbers
#define STORAGE_BLK_NBR 45  // Number of blocks
#define STORAGE_BLK_SIZ 512 // Size of each block in bytes

/**
 * Initializes the USB module, must be called before using this module
 */
void usb_init();

/**
 * Handles a read operation (simulates a storage device)
 */
void usb_read(uint8_t *buffer, uint32_t block_address, uint16_t block_count);

/**
 * Handles a write operation (simulates a storage device)
 */
void usb_write(uint8_t *buffer, uint32_t block_address, uint16_t block_count);

#endif
