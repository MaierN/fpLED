
#include "stm32f1xx_hal.h"

#include "usb_device.h"

#include "usb.h"

#include "leds/leds.h"

void usb_init() {
    MX_USB_DEVICE_Init();
}
