
#ifndef LEDS_H
#define LEDS_H

void leds_init();
void leds_send();
void leds_wait_sent();
volatile uint8_t* leds_get_current_buffer();

#endif
