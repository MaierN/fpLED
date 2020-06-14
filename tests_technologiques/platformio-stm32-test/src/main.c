
#include "stm32f1xx_hal.h"

#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "exception_handler/exception_handler.h"
#include "leds/leds.h"
#include "usb/usb.h"

void system_clock_init();
void gpio_init();

size_t get_pos_pixel(uint32_t x, uint32_t y) {
    return 8 * y + (y % 2 == 0 ? x : 7 - x);
}

int main() {
    HAL_Init();

    system_clock_init();
    gpio_init();

    leds_init();
    usb_init();

#define DATA_LENGTH 8
    uint8_t data[DATA_LENGTH] = {1, 1, 2, 3, 5, 8, 13, 21};

    while (1) {
        /*HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

        for (int i = 1; i < 64; i++) {
            leds_set_pixel(0, i, 10, 1, 16);
            leds_set_pixel(0, i-1, 0, 0, 0);
            HAL_Delay(50);
        }
        leds_set_pixel(0, 63, 0, 0, 0);*/

        /*
        for (int i = 0; i < 4; i++) {
            leds_set_pixel(0, get_pos_pixel(3, i+3), 4, 4, 0);
            leds_set_pixel(1, get_pos_pixel(3, i+3), 4, 0, 4);
            //HAL_Delay(250);
            leds_set_pixel(0, get_pos_pixel(3, i+3), 0, 0, 0);
            leds_set_pixel(1, get_pos_pixel(3, i+3), 0, 0, 0);
        }
        */

        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        uint8_t res = CDC_Transmit_FS(data, DATA_LENGTH);
        if (res != USBD_OK) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            HAL_Delay(500);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        }

        HAL_Delay(1000);
    }
}

void system_clock_init() {
    // useful information about this function : https://www.onetransistor.eu/2018/09/stm32cube-code-init-bluepill.html

    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    // enable HSE (from crystal, 8 MHz) clock, without prediv (/1)
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;

    // enable HSI (internal RC oscillator) clock
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;

    // enable and configure PLL to use HSE as clock source, with x9 multiplier to obtain 72 MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        exception_handler_default_error();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        exception_handler_default_error();
    }

    // configure USB prescaler, /1.5 to obtain 48 MHz
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        exception_handler_default_error();
    }
}

void gpio_init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
