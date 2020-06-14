
#include <stdbool.h>

#include "stm32f1xx_hal.h"

#include "leds.h"

#include "exception_handler/exception_handler.h"

#define LED_PORT GPIOA
#define LED_PORT_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE

#define STRIP_N 5    // number of LED strip
#define SLICE_N 128  // number of slice in each strip
#define LED_N 2      // number of led in each slice
#define LED_BYTE_N 3 // number of byte in each led

uint16_t strip_pins[STRIP_N] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};

#define CIRCULAR true
#define LED_RESET_PERIOD_COUNT 50

TIM_HandleTypeDef tim_2;
TIM_OC_InitTypeDef tim_2_oc_0;
TIM_OC_InitTypeDef tim_2_oc_1;

#define TIM_2_PERIOD (SystemCoreClock / 800000) // 1250 [ns] period = 1/800000 [s]

DMA_HandleTypeDef dma_up;
DMA_HandleTypeDef dma_down_0;
DMA_HandleTypeDef dma_down_1;

uint32_t dma_up_down_bit_source[] = {0xffffffff}; // mask to select every strip

uint16_t led_bits_to_send[8 * LED_BYTE_N * LED_N];

uint8_t strip_buffers[LED_BYTE_N * LED_N * SLICE_N][STRIP_N];

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

volatile uint32_t slice_counter = 0;
uint32_t reset_counter = 0;

static void load_next_slice_half(bool first_half) {
    size_t to_send_index = first_half ? 0 : ARRAY_SIZE(led_bits_to_send) / 2;
    size_t buffer_index = slice_counter * LED_N + (first_half ? 0 : LED_N / 2);

    for (size_t i = 0; i < LED_N / 2; i++) {
        for (size_t j = 0; j < LED_BYTE_N; j++) {
            for (int k = 0; k < 8; k++) {
                uint8_t send_byte = 0xff;
                for (size_t l = 0; l < STRIP_N; l++) {
                    uint8_t curr_byte = strip_buffers[(buffer_index + i) * LED_BYTE_N + j][l];

                    // put 1 to send 0 bit, put 0 to send 1 bit
                    send_byte &= ((curr_byte >> (7 - k)) & 0x1) ? ~strip_pins[l] : 0xff;
                }
                led_bits_to_send[to_send_index + i * LED_BYTE_N * 8 + j * 8 + k] = send_byte;
            }
        }
    }
}

static void send() {
    slice_counter = 0;
    load_next_slice_half(false);
    load_next_slice_half(true);

    // clear all DMA flags
    __HAL_DMA_CLEAR_FLAG(&dma_up, DMA_FLAG_TC2 | DMA_FLAG_HT2 | DMA_FLAG_TE2);
    __HAL_DMA_CLEAR_FLAG(&dma_down_0, DMA_FLAG_TC5 | DMA_FLAG_HT5 | DMA_FLAG_TE5);
    __HAL_DMA_CLEAR_FLAG(&dma_down_1, DMA_FLAG_TC7 | DMA_FLAG_HT7 | DMA_FLAG_TE7);

    // configure the number of bytes to be transferred by the DMA controller
    dma_up.Instance->CNDTR = ARRAY_SIZE(led_bits_to_send);
    dma_down_0.Instance->CNDTR = ARRAY_SIZE(led_bits_to_send);
    dma_down_1.Instance->CNDTR = ARRAY_SIZE(led_bits_to_send);

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
    /*
#define TEST_COUNT 100
    static uint32_t test_counter = 0;
    test_counter++;
    if (test_counter > TEST_COUNT * 2) {
        test_counter = 0;
    }
    if (test_counter > TEST_COUNT) {
        led_bits_to_send[72 + 4] = 0x0;
    } else {
        led_bits_to_send[72 + 4] = strip_pins[0];
    }*/
}

static void dma_transfer_half_complete_handler(DMA_HandleTypeDef *dma_handle) {
    slice_counter++;
    if (slice_counter < SLICE_N) {
        load_next_slice_half(true);
    }
}

static void dma_transfer_complete_handler(DMA_HandleTypeDef *dma_handle) {
    if (slice_counter < SLICE_N) {
        load_next_slice_half(false);
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
        LED_PORT->BRR = dma_up_down_bit_source[0];
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

        // todo test
        send();
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

    uint16_t all_pins = 0x0;
    for (size_t i = 0; i < STRIP_N; i++) {
        all_pins |= strip_pins[i];
    }
    HAL_GPIO_WritePin(LED_PORT, all_pins, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = all_pins;
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
    dma_up.Init.Mode = CIRCULAR ? DMA_CIRCULAR : DMA_NORMAL;
    dma_up.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_up);
    HAL_DMA_Start(&dma_up, (uint32_t)dma_up_down_bit_source, (uint32_t)&LED_PORT->BSRR, ARRAY_SIZE(led_bits_to_send));

    // TIM2 CC1: dma down (0 bit)
    dma_down_0.Instance = DMA1_Channel5;
    dma_down_0.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_down_0.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_down_0.Init.MemInc = DMA_MINC_ENABLE;
    dma_down_0.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_down_0.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dma_down_0.Init.Mode = CIRCULAR ? DMA_CIRCULAR : DMA_NORMAL;
    dma_down_0.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_down_0);
    HAL_DMA_Start(&dma_down_0, (uint32_t)led_bits_to_send, (uint32_t)&LED_PORT->BRR, ARRAY_SIZE(led_bits_to_send));

    // TIM2 CC2: dma down (1 bit)
    dma_down_1.Instance = DMA1_Channel7;
    dma_down_1.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_down_1.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_down_1.Init.MemInc = DMA_MINC_DISABLE;
    dma_down_1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_down_1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dma_down_1.Init.Mode = CIRCULAR ? DMA_CIRCULAR : DMA_NORMAL;
    dma_down_1.Init.Priority = DMA_PRIORITY_LOW;

    HAL_DMA_Init(&dma_down_1);
    dma_down_1.XferCpltCallback = dma_transfer_complete_handler;
    dma_down_1.XferHalfCpltCallback = dma_transfer_half_complete_handler;
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    HAL_DMA_Start_IT(&dma_down_1, (uint32_t)dma_up_down_bit_source, (uint32_t)&LED_PORT->BRR, ARRAY_SIZE(led_bits_to_send));
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
    for (size_t i = 0; i < LED_BYTE_N * LED_N * SLICE_N; i++) {
        for (size_t j = 0; j < STRIP_N; j++) {
            strip_buffers[i][j] = 0;
        }
    }

    leds_gpio_init();
    HAL_Delay(1000);

    leds_dma_init();
    leds_timer_init();

    send();

    size_t i = 0;
    while (1) {
        size_t next_i = (i + 1) % (LED_BYTE_N * LED_N * SLICE_N);
        strip_buffers[next_i][0] = 4;
        strip_buffers[i][0] = 0;
        i = next_i;
        HAL_Delay(50);
    }
}
