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
 * Project: HEIA-FR / Fast ws281x LED control in parallel via USB
 *
 * Purpose: This module is a driver for the USB LED control device
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

// API inspired from https://github.com/jgarff/rpi_ws281x/blob/master/ws2811.h

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t led_controller_led_t; // LED RGB color : 0x00RrGgBb

// struct describing a LED strip
typedef struct {
    led_controller_led_t* leds;
} led_controller_channel_t;

// struct describing a LED controller
typedef struct {
    char* device_path; // path to the mounted device
    size_t channel_n; // number of parallel channels (strips)
    size_t led_n; // number of LEDs on each channel (strip)
    bool do_gamma_correction; // enables or disables the gamma color correction
    led_controller_channel_t* channels; // channel data
} led_controller_t;

/**
 * Sets up the LED controller and the device (all fields of lc except "channels" must be set before the call)
 */
void led_controller_init(led_controller_t* lc);

/**
 * Frees resources (must be called when the lc won't be used anymore)
 */
void led_controller_cleanup(led_controller_t* lc);

/**
 * Sends the prepared data to the device
 */
void led_controller_render(led_controller_t* lc);

#endif
