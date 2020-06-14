
#include "stm32f1xx_hal.h"

#include "exception_handler.h"

void exception_handler_default_error() {
    volatile uint32_t count = 0;
    while (1) {
        count++;
    }
}

void SysTick_Handler(void) {
    HAL_IncTick();
}

void NMI_Handler(void) {}

void HardFault_Handler(void) {
    exception_handler_default_error();
}

void MemManage_Handler(void) {
    exception_handler_default_error();
}

void BusFault_Handler(void) {
    exception_handler_default_error();
}

void UsageFault_Handler(void) {
    exception_handler_default_error();
}

void SVC_Handler(void) {}

void DebugMon_Handler(void) {}

void PendSV_Handler(void) {}