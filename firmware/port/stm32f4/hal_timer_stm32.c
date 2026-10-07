/**
 * @file hal_timer_stm32.c
 * @brief STM32F4xx Hardware Implementation for System Timing & SysTick
 * @author Sherifred Singh
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_timer.h"
#include "stm32f4xx_hal.h"

void hal_timer_init(void)
{
    /* SysTick is initialized by HAL_Init() */
}

uint32_t hal_timer_get_ms(void)
{
    return HAL_GetTick();
}

void hal_timer_delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = us * (SystemCoreClock / 1000000U);
    while ((DWT->CYCCNT - start) < ticks);
}

void hal_timer_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

#endif /* TARGET_STM32F4 */
