
#ifndef LEDS_H
#define LEDS_H

void leds_init();
void leds_set_pixel(size_t strip, size_t pixel, uint8_t r, uint8_t g, uint8_t b);

#endif
