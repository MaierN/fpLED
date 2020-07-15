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
 * Purpose: This module defines handlers for the various interrupts, and provides a default error handler
 *
 * Author:  Nicolas Maier
 * Date:    June 2020
 */

#include "stm32f1xx_hal.h"

#include "exception_handler.h"

void exception_handler_default_error() {
    // note: for production use, maybe this function should reset the MCU automatically instead of blocking the program
    volatile uint32_t count = 0;
    while (1) {
        count++;
    }
}

// Error interrupt handlers
void HardFault_Handler(void) {
    exception_handler_default_error();
}
void MemManage_Handler(void) {
    exception_handler_default_error();
}
void BusFault_Handler(void) {
    exception_handler_default_error();
}
void UsageFault_Handler(void) {
    exception_handler_default_error();
}
