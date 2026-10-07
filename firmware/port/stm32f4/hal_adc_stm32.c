/**
 * @file hal_adc_stm32.c
 * @brief STM32F4xx Hardware Implementation for Multi-Channel ADC (ADC1 with DMA)
 * @author Sherifred Singh
 */

#ifdef TARGET_STM32F4

#include "../../hal/hal_adc.h"
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef hadc1;
static uint16_t s_stm32_adc_buffer[HAL_ADC_CH_COUNT];

void hal_adc_init(void)
{
    /* Start DMA circular conversion */
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)s_stm32_adc_buffer, HAL_ADC_CH_COUNT);
}

uint16_t hal_adc_read_raw(hal_adc_channel_t ch)
{
    if (ch < HAL_ADC_CH_COUNT) {
        return s_stm32_adc_buffer[ch];
    }
    return 0;
}

uint16_t hal_adc_read_mv(hal_adc_channel_t ch)
{
    if (ch < HAL_ADC_CH_COUNT) {
        return (uint16_t)(((uint32_t)s_stm32_adc_buffer[ch] * 3300U) / 4095U);
    }
    return 0;
}

#endif /* TARGET_STM32F4 */
