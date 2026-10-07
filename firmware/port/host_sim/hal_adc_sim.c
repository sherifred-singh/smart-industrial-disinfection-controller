/**
 * @file hal_adc_sim.c
 * @brief Host Simulation Implementation for Multi-Channel ADC
 * @author Sherifred Singh
 */

#include "hal_adc.h"
#include <string.h>

static uint16_t s_adc_raw[HAL_ADC_CH_COUNT];

void hal_adc_init(void)
{
    s_adc_raw[HAL_ADC_CH_CHAMBER_TEMP]        = 931U;  /* ~25.0 C */
    s_adc_raw[HAL_ADC_CH_CHAMBER_HUMIDITY]    = 2048U; /* ~50% RH */
    s_adc_raw[HAL_ADC_CH_PROCESS_CONC]        = 0U;
    s_adc_raw[HAL_ADC_CH_AMBIENT_SAFETY_LEAK] = 0U;
    s_adc_raw[HAL_ADC_CH_SUPPLY_VOLTAGE]      = 2978U;
}

uint16_t hal_adc_read_raw(hal_adc_channel_t ch)
{
    if (ch < HAL_ADC_CH_COUNT) {
        return s_adc_raw[ch];
    }
    return 0;
}

uint16_t hal_adc_read_mv(hal_adc_channel_t ch)
{
    if (ch < HAL_ADC_CH_COUNT) {
        return (uint16_t)(((uint32_t)s_adc_raw[ch] * 3300U) / 4095U);
    }
    return 0;
}

void hal_adc_sim_set_raw(hal_adc_channel_t ch, uint16_t raw_count)
{
    if (ch < HAL_ADC_CH_COUNT) {
        if (raw_count > 4095U) {
            raw_count = 4095U;
        }
        s_adc_raw[ch] = raw_count;
    }
}

void hal_adc_sim_set_mv(hal_adc_channel_t ch, uint16_t mv)
{
    if (ch < HAL_ADC_CH_COUNT) {
        if (mv > 3300U) {
            mv = 3300U;
        }
        s_adc_raw[ch] = (uint16_t)(((uint32_t)mv * 4095U) / 3300U);
    }
}
