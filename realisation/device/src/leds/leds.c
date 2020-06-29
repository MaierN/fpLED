
#include <stdbool.h>
#include <string.h>

#include "stm32f1xx_hal.h"

#include "leds.h"

#include "exception_handler/exception_handler.h"

#define BITBANDING_BASE_ADDRESS 0x20000000
#define BITBANDING_BITBAND_ADDRESS 0x22000000
#define BITBANDING_GET_ADDRESS(var_address, bit_offset) ((volatile uint32_t *) (BITBANDING_BITBAND_ADDRESS + ((uint32_t)(var_address - BITBANDING_BASE_ADDRESS) << 5) + (bit_offset << 2)))

#define LED_PORT GPIOA
#define LED_PORT_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE

#define TIM_2_PERIOD (SystemCoreClock / 800000) // 1250 [ns] period = 1/800000 [s]

TIM_HandleTypeDef tim_2;
TIM_OC_InitTypeDef tim_2_oc_0;
TIM_OC_InitTypeDef tim_2_oc_1;

DMA_HandleTypeDef dma_up;
DMA_HandleTypeDef dma_down_0;
DMA_HandleTypeDef dma_down_1;

uint8_t strip_pins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
uint32_t all_pins[] = {0xffffffff}; // mask to select every pins

volatile uint8_t usb_bit_buffer[6 * 512];

#define TEST_A 2
volatile uint8_t led_dma_buffer[8 * LED_BYTE_N * 2 * TEST_A];
volatile size_t led_dma_count = 0;

volatile uint32_t reset_counter = 0;

volatile bool image_shown = true; // true if finished showing image


/*static void prepare_showing_buffer() {
    for (size_t strip = 0; strip < 8; strip++) {

        volatile uint32_t* bitband_addr = BITBANDING_GET_ADDRESS(led_bit_buffer, strip);

        volatile uint8_t* curr_usb_buffer = usb_bit_buffer + strip * LED_N * LED_BYTE_N / 2;

        for (size_t i = 0; i < LED_N * LED_BYTE_N; i++) {
            uint8_t byte = (~*(curr_usb_buffer + i/2)) << (4 * (i % 2));

            *bitband_addr = byte >> 7; bitband_addr += 8;
            *bitband_addr = byte >> 6; bitband_addr += 8;
            *bitband_addr = byte >> 5; bitband_addr += 8;
            *bitband_addr = byte >> 4; bitband_addr += 8;
            bitband_addr += 32;
        }
    }
}*/

static void prepare_dma_buffer_half() {
    size_t a = led_dma_count % 2 == 0 ? 0 : 8 * LED_BYTE_N * TEST_A;

    for (size_t strip = 0; strip < 8; strip++) {
        volatile uint32_t* bitband_addr = BITBANDING_GET_ADDRESS(led_dma_buffer + a, strip);

        volatile uint8_t* curr_usb_buffer = usb_bit_buffer + led_dma_count * LED_BYTE_N  + strip * LED_N * LED_BYTE_N / 2;

        for (size_t i = 0; i < LED_BYTE_N * TEST_A; i++) {
            uint8_t byte = (~*(curr_usb_buffer + i/2)) << (4 * (i % 2));

            *bitband_addr = byte >> 7; bitband_addr += 8;
            *bitband_addr = byte >> 6; bitband_addr += 8;
            *bitband_addr = byte >> 5; bitband_addr += 8;
            *bitband_addr = byte >> 4; bitband_addr += 8;
            bitband_addr += 32;
        }
    }

    led_dma_count++;
}

static void dma_transfer_half_complete_handler(DMA_HandleTypeDef *dma_handle) {
    if (led_dma_count < LED_N/TEST_A) {
        prepare_dma_buffer_half();
    }
}

static void dma_transfer_complete_handler(DMA_HandleTypeDef *dma_handle) {
    if (led_dma_count < LED_N/TEST_A) {
        prepare_dma_buffer_half();
    } else {
        // disable DMA transfer
        __HAL_DMA_DISABLE(&dma_up);
        __HAL_DMA_DISABLE(&dma_down_0);
        __HAL_DMA_DISABLE(&dma_down_1);

        // disable DMA requests of TIM2
        __HAL_TIM_DISABLE_DMA(&tim_2, TIM_DMA_UPDATE);
        __HAL_TIM_DISABLE_DMA(&tim_2, TIM_DMA_CC1);
        __HAL_TIM_DISABLE_DMA(&tim_2, TIM_DMA_CC2);

        // enable TIM2 update interrupts (will be used to time the RST signal > 50 us)
        __HAL_TIM_ENABLE_IT(&tim_2, TIM_IT_UPDATE);

        // set pin to low, for RST signal
        LED_PORT->BRR = all_pins[0];
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    // wait 50 us for RST signal
    if (reset_counter < LED_RESET_PERIOD_COUNT) {
        reset_counter++;
    } else {
        reset_counter = 0;
        // disable timer
        __HAL_TIM_DISABLE(&tim_2);
        TIM2->CR1 = 0;

        // disable TIM2 update interrupts
        __HAL_TIM_DISABLE_IT(&tim_2, TIM_IT_UPDATE);

        image_shown = true;
    }
}

void DMA1_Channel7_IRQHandler(void) {
    // clear flags, and call appropriate callbacks
    HAL_DMA_IRQHandler(&dma_down_1);
}

void TIM2_IRQHandler(void) {
    // clear flags, and call appropriate callbacks
    HAL_TIM_IRQHandler(&tim_2);
}

static void leds_gpio_init() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    LED_PORT_ENABLE();

    uint8_t all_enabled_pins = 0x0;
    for (size_t i = 0; i < ARRAY_SIZE(strip_pins); i++) {
        all_enabled_pins |= strip_pins[i];
    }
    HAL_GPIO_WritePin(LED_PORT, all_enabled_pins, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = all_enabled_pins;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
}

static void leds_dma_init() {
    __HAL_RCC_DMA1_CLK_ENABLE();

    // TIM2 Update: dma up
    dma_up.Instance = DMA1_Channel2;
    dma_up.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_up.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_up.Init.MemInc = DMA_MINC_DISABLE;
    dma_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_up.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_up.Init.Mode = DMA_CIRCULAR;
    dma_up.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_up);
    HAL_DMA_Start(&dma_up, (uint32_t)all_pins, (uint32_t)&LED_PORT->BSRR, ARRAY_SIZE(led_dma_buffer));

    // TIM2 CC1: dma down (0 bit)
    dma_down_0.Instance = DMA1_Channel5;
    dma_down_0.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_down_0.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_down_0.Init.MemInc = DMA_MINC_ENABLE;
    dma_down_0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_down_0.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_down_0.Init.Mode = DMA_CIRCULAR;
    dma_down_0.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_down_0);
    HAL_DMA_Start(&dma_down_0, (uint32_t)led_dma_buffer, (uint32_t)&LED_PORT->BRR, ARRAY_SIZE(led_dma_buffer));

    // TIM2 CC2: dma down (1 bit)
    dma_down_1.Instance = DMA1_Channel7;
    dma_down_1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_down_1.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_down_1.Init.MemInc = DMA_MINC_DISABLE;
    dma_down_1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_down_1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_down_1.Init.Mode = DMA_CIRCULAR;
    dma_down_1.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_down_1);
    dma_down_1.XferHalfCpltCallback = dma_transfer_half_complete_handler;
    dma_down_1.XferCpltCallback = dma_transfer_complete_handler;
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    HAL_DMA_Start_IT(&dma_down_1, (uint32_t)all_pins, (uint32_t)&LED_PORT->BRR, ARRAY_SIZE(led_dma_buffer));
}

static void leds_timer_init() {
    __HAL_RCC_TIM2_CLK_ENABLE();

    tim_2.Instance = TIM2;
    tim_2.Init.Period = TIM_2_PERIOD;
    tim_2.Init.Prescaler = 0;
    tim_2.Init.RepetitionCounter = 0;
    tim_2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_PWM_Init(&tim_2);

    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    tim_2_oc_0.OCMode = TIM_OCMODE_PWM1;
    tim_2_oc_0.OCPolarity = TIM_OCPOLARITY_HIGH;
    tim_2_oc_0.Pulse = TIM_2_PERIOD * 32 / 100; // ws2812b datasheet -> ~ 400 ns high time for '0' bit
    tim_2_oc_0.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&tim_2, &tim_2_oc_0, TIM_CHANNEL_1);

    tim_2_oc_1.OCMode = TIM_OCMODE_PWM1;
    tim_2_oc_1.OCPolarity = TIM_OCPOLARITY_HIGH;
    tim_2_oc_1.Pulse = TIM_2_PERIOD * 64 / 100; // ws2812b datasheet -> ~ 800 ns high time for '1' bit
    tim_2_oc_1.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&tim_2, &tim_2_oc_1, TIM_CHANNEL_2);

    HAL_TIM_Base_Start(&tim_2);
    HAL_TIM_PWM_Start(&tim_2, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&tim_2, TIM_CHANNEL_2);
}

void leds_init() {
    memset((void*)led_dma_buffer, 0xff, ARRAY_SIZE(led_dma_buffer));
    memset((void*)usb_bit_buffer, 0xff, ARRAY_SIZE(usb_bit_buffer));

    leds_gpio_init();
    HAL_Delay(1000);

    leds_dma_init();
    leds_timer_init();
}

void leds_send() {
    image_shown = false;

    led_dma_count = 0;
    prepare_dma_buffer_half();
    prepare_dma_buffer_half();

    // clear all DMA flags
    __HAL_DMA_CLEAR_FLAG(&dma_up, DMA_FLAG_TC2 | DMA_FLAG_HT2 | DMA_FLAG_TE2);
    __HAL_DMA_CLEAR_FLAG(&dma_down_0, DMA_FLAG_TC5 | DMA_FLAG_HT5 | DMA_FLAG_TE5);
    __HAL_DMA_CLEAR_FLAG(&dma_down_1, DMA_FLAG_TC7 | DMA_FLAG_HT7 | DMA_FLAG_TE7);

    // configure the number of bytes to be transferred by the DMA controller
    dma_up.Instance->CNDTR = ARRAY_SIZE(led_dma_buffer);
    dma_down_0.Instance->CNDTR = ARRAY_SIZE(led_dma_buffer);
    dma_down_1.Instance->CNDTR = ARRAY_SIZE(led_dma_buffer);


    // clear all TIM2 flags
    __HAL_TIM_CLEAR_FLAG(&tim_2, TIM_FLAG_UPDATE | TIM_FLAG_CC1 | TIM_FLAG_CC2 | TIM_FLAG_CC3 | TIM_FLAG_CC4);

    __HAL_DMA_ENABLE(&dma_up);
    __HAL_DMA_ENABLE(&dma_down_0);
    __HAL_DMA_ENABLE(&dma_down_1);

    __HAL_TIM_ENABLE_DMA(&tim_2, TIM_DMA_UPDATE);
    __HAL_TIM_ENABLE_DMA(&tim_2, TIM_DMA_CC1);
    __HAL_TIM_ENABLE_DMA(&tim_2, TIM_DMA_CC2);

    TIM2->CNT = TIM_2_PERIOD - 1;

    __HAL_TIM_ENABLE(&tim_2);
}

void leds_wait_sent() {
    while (!image_shown) {}
}
