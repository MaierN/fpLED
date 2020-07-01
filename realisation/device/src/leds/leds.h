
#ifndef LEDS_H
#define LEDS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define STRIP_N 8    // number of LED strips in parallel
#define LED_N 256    // number of LED on each pin
#define LED_BYTE_N 3 // number of byte in each LED

#define LED_RESET_PERIOD_COUNT 50

volatile uint8_t* leds_usb_bit_buffer;
void leds_init();
void leds_send();
void leds_wait_sent();

#endif
