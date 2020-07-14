
// API inspired from https://github.com/jgarff/rpi_ws281x/blob/master/ws2811.h

#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint32_t led_controller_led_t;

typedef struct {
    led_controller_led_t* leds;
} led_controller_channel_t;

typedef struct {
    char* device_path;
    size_t channel_n;
    size_t led_n;
    bool do_gamma_correction;
    led_controller_channel_t* channels;
} led_controller_t;

void led_controller_init(led_controller_t* lc);

void led_controller_cleanup(led_controller_t* lc);

void led_controller_render(led_controller_t* lc);

#endif
