
#ifndef LEDS_H
#define LEDS_H

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define LED_N 256    // number of led on each pin
#define LED_BYTE_N 3 // number of byte in each led

#define LED_RESET_PERIOD_COUNT 50

void leds_init();
void leds_send();
void leds_wait_sent();
volatile uint8_t* leds_get_buffer();

#endif
