/*
  WS2812B CPU and memory efficient library
  Date: 28.9.2016
  Author: Martin Hubacek
  	  	  http://www.martinhubacek.cz
  	  	  @hubmartin
  Licence: MIT License
*/

/**
 * Project: HEIA-FR / Accélérateur de tour télécom
 *
 * Purpose: This module sends color codes on GPIO pins to control ws281x LED strips
 * 
 * This module was written by Nicolas Maier, based on Martin Hubacek's work (see original header above)
 * reference source code used to make this module: https://github.com/hubmartin/WS2812B_STM32F103/blob/master/src/ws2812b.c
 * post explaining principles behind this code: http://www.martinhubacek.cz/arm/improved-stm32-ws2812b-library
 * 
 * What was copied or inspired from Martin Hubacek's work:
 * - Timer, DMA and GPIO configuration and usage with their specific registers
 * - Idea to use 3 DMA channels to generate the signal, with 3 trigger sources from the timer, as explained in the post linked above
 * - Usage of DMA transfer completion handles ("half complete" and "complete") to prepare DMA buffer
 * - Usage of "bit banding" to prepare DMA buffer quickly
 * 
 * Modifications/improvements:
 * - Removed unused parts, clean code
 * - More explicit variable names
 * - Added double buffer mechanic
 * - Added compression/decompression methods for more efficient data transfer and storage
 * - Various specific adjustments to fit the project's needs
 */

#ifndef LEDS_H
#define LEDS_H

volatile uint8_t* leds_usb_bit_buffer; // buffer to be filled by the usb module with LED color compressed data

/**
 * Initializes the leds module, must be called before using this module
 */
void leds_init();

/**
 * Sends the data stored in leds_usb_bit_buffer to the LED strips
 */
void leds_send();

/**
 * Waits for the sending to end, should be called before calling leds_send or changing parameters
 */
void leds_wait_sent();

/**
 * Changes the compression mode, will change how the data in leds_usb_bit_buffer is interpreted/decompressed
 */
void leds_set_compression_mode(uint8_t mode);

#endif
