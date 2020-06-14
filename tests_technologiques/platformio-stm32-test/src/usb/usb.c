
#include "stm32f1xx_hal.h"

#include "usb_device.h"

#include "usb.h"

#include "leds/leds.h"

void usb_handle_receive(uint8_t *Buf, uint32_t *len) {
    if (Buf[0] == '1') {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 0);
        leds_set_pixel(0, 4, 10, 10, 0);
    } else if (Buf[0] == '0') {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, 1);
        leds_set_pixel(0, 4, 0, 0, 0);
    }
}

void usb_init() {
    MX_USB_DEVICE_Init();
}
