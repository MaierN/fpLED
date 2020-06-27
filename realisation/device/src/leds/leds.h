
#ifndef LEDS_H
#define LEDS_H

extern volatile uint8_t led_bit_buffer[];
void leds_init();
void leds_send();
void leds_wait_sent();

#endif
