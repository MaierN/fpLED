
#include "driver/led_controller.h"

#include <stdio.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("%s path_to_device\n", argv[0]);
        return 0;
    }

    led_controller_t lc = {
        argv[1], // char* device_path
        8,       // size_t channel_n
        256,     // size_t led_n
        true,    // bool do_gamma_correction
        0,       // led_controller_channel_t* channels
    };

    led_controller_init(&lc);

    lc.channels[0].leds[0] = 0x00ff0000;
    lc.channels[0].leds[10] = 0x00400000;
    lc.channels[1].leds[3] = 0x0000ff00;
    lc.channels[3].leds[5] = 0x000000ff;

    led_controller_render(&lc);

    led_controller_cleanup(&lc);

    return 0;
}
