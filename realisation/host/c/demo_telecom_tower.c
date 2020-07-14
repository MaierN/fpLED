
#include "driver/led_controller.h"
#include "driver/neopixel_matrix.h"

#include <stdio.h>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

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

    neopixel_matrix_t matrices[] = {
        {&lc, 0, 0, 8, 32},
        {&lc, 1, 0, 8, 32},
        {&lc, 2, 0, 8, 32},
        {&lc, 3, 0, 8, 32},
        {&lc, 4, 0, 8, 8},
        {&lc, 4, 8*8, 8, 8},
    };

    uint32_t color = 0x00400040;

    size_t curr_matrix = 0;
    size_t pos = 0;
    size_t pos_time = 0;
    
    for (size_t i = 0; i < 8; i++) {
        neopixel_matrix_set_pixel_color(&(matrices[curr_matrix]), i, pos, color);
    }
    for(;;) {
        pos_time++;
        if (pos_time >= 5) {
            for (size_t i = 0; i < 8; i++) {
                neopixel_matrix_set_pixel_color(&(matrices[curr_matrix]), i, pos, 0x00000000);
            }

            pos++;
            if (pos >= matrices[curr_matrix].y_size) {
                pos = 0;
                curr_matrix++;
                if (curr_matrix >= ARRAY_SIZE(matrices)) {
                    curr_matrix = 0;
                }
            }
            
            for (size_t i = 0; i < 8; i++) {
                neopixel_matrix_set_pixel_color(&(matrices[curr_matrix]), i, pos, color);
            }

            pos_time = 0;
        }

        led_controller_render(&lc);
    }

    led_controller_cleanup(&lc);

    return 0;
}
