/**
 * @file hal_watchdog_stm32.c
 * @brief STM32F4xx Hardware Implementation for Independent Watchdog (IWDG)
 * @author Sherifred Singh
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_watchdog.h"
#include "stm32f4xx_hal.h"

static IWDG_HandleTypeDef s_hiwdg;

void hal_watchdog_init(uint32_t timeout_ms)
{
    /* LSI is typically 32 kHz -> Prescaler 32 gives 1 kHz (1ms per tick) */
    s_hiwdg.Instance       = IWDG;
    s_hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
    s_hiwdg.Init.Reload    = (timeout_ms > 4095U) ? 4095U : (uint32_t)timeout_ms;

    HAL_IWDG_Init(&s_hiwdg);
}

void hal_watchdog_feed(void)
{
    HAL_IWDG_Refresh(&s_hiwdg);
}

#endif /* TARGET_STM32F4 */
