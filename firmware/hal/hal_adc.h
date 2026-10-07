/**
 * @file hal_adc.h
 * @brief Hardware Abstraction Layer for Multi-Channel ADC
 * @author Sherifred Singh
 * 
 * Hardware-agnostic interface for converting analog sensor voltages
 * (temperature, humidity, environmental/ambient gas concentration).
 */

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ADC Logical Channel Enumeration
 */
typedef enum {
    HAL_ADC_CH_CHAMBER_TEMP = 0,   /**< Chamber Internal Temperature Sensor */
    HAL_ADC_CH_CHAMBER_HUMIDITY,   /**< Chamber Relative Humidity Sensor */
    HAL_ADC_CH_PROCESS_CONC,       /**< Chamber Active Disinfection/Gas Concentration */
    HAL_ADC_CH_AMBIENT_SAFETY_LEAK,/**< External Enclosure Safety Leak Detector */
    HAL_ADC_CH_SUPPLY_VOLTAGE,     /**< 24V DC Internal Rail Monitor */
    HAL_ADC_CH_COUNT
} hal_adc_channel_t;

/**
 * @brief Initialize the ADC peripheral (DMA / Continuous scan / Polling).
 */
void hal_adc_init(void);

/**
 * @brief Read the raw 12-bit ADC value (0 - 4095) of a specific channel.
 * @param ch Channel identifier
 * @return Raw 12-bit ADC count
 */
uint16_t hal_adc_read_raw(hal_adc_channel_t ch);

/**
 * @brief Read the calculated millivolt value (0 - 3300 mV) of a specific channel.
 * @param ch Channel identifier
 * @return Voltage in millivolts
 */
uint16_t hal_adc_read_mv(hal_adc_channel_t ch);

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_H */
