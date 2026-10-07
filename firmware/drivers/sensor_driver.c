/**
 * @file sensor_driver.c
 * @brief Industrial Multi-Sensor Acquisition & Signal Conditioning Driver Implementation
 * @author Sherifred Singh
 */

#include "sensor_driver.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "filter_service.h"
#include "alarm_fault_manager.h"
#include "config_manager.h"
#include <string.h>

#define DEBOUNCE_THRESHOLD_MS   30U

static sensor_data_t    s_current_sensors;
static ema_filter_t     s_temp_filter;
static ema_filter_t     s_humidity_filter;
static ema_filter_t     s_conc_filter;
static ema_filter_t     s_leak_filter;
static median3_filter_t s_temp_median;

static uint16_t s_start_btn_cnt = 0;
static uint16_t s_stop_btn_cnt  = 0;
static bool     s_prev_start_raw = false;
static bool     s_prev_stop_raw  = false;

void sensor_driver_init(void)
{
    memset(&s_current_sensors, 0, sizeof(sensor_data_t));

    filter_ema_init(&s_temp_filter, 20);
    filter_ema_init(&s_humidity_filter, 20);
    filter_ema_init(&s_conc_filter, 25);
    filter_ema_init(&s_leak_filter, 30);
    filter_median3_init(&s_temp_median);

    s_current_sensors.temp_sensor_valid = true;
    s_current_sensors.conc_sensor_valid = true;
    s_current_sensors.is_estop_normal   = true;
    s_current_sensors.is_door_closed    = true;
    s_current_sensors.is_airflow_ok     = true;
}

void sensor_driver_process_10ms(uint32_t elapsed_ms)
{
    uint16_t mv_temp     = hal_adc_read_mv(HAL_ADC_CH_CHAMBER_TEMP);
    uint16_t mv_humidity = hal_adc_read_mv(HAL_ADC_CH_CHAMBER_HUMIDITY);
    uint16_t mv_conc     = hal_adc_read_mv(HAL_ADC_CH_PROCESS_CONC);
    uint16_t mv_leak     = hal_adc_read_mv(HAL_ADC_CH_AMBIENT_SAFETY_LEAK);
    uint16_t mv_supply   = hal_adc_read_mv(HAL_ADC_CH_SUPPLY_VOLTAGE);

    if (mv_temp < 50U || mv_temp > 3250U) {
        s_current_sensors.temp_sensor_valid = false;
        alarm_fault_set(FAULT_CODE_SENSOR_DISCONNECTED, FAULT_SEV_CRITICAL);
    } else {
        s_current_sensors.temp_sensor_valid = true;
    }

    uint16_t median_temp_mv = filter_median3_update(&s_temp_median, mv_temp);
    uint16_t filtered_temp_mv = filter_ema_update(&s_temp_filter, median_temp_mv);
    uint16_t filtered_hum_mv  = filter_ema_update(&s_humidity_filter, mv_humidity);
    uint16_t filtered_conc_mv = filter_ema_update(&s_conc_filter, mv_conc);
    uint16_t filtered_leak_mv = filter_ema_update(&s_leak_filter, mv_leak);

    if (filtered_temp_mv >= 500U) {
        s_current_sensors.chamber_temp_c10 = (int16_t)(((int32_t)filtered_temp_mv - 500) * 10 / 10);
    } else {
        s_current_sensors.chamber_temp_c10 = (int16_t)(((int32_t)filtered_temp_mv - 500) * 10 / 10);
    }

    s_current_sensors.chamber_humidity_rh = (uint16_t)(((uint32_t)filtered_hum_mv * 100U) / 3300U);
    if (s_current_sensors.chamber_humidity_rh > 100U) {
        s_current_sensors.chamber_humidity_rh = 100U;
    }

    s_current_sensors.chamber_conc_ppm = (uint16_t)(((uint32_t)filtered_conc_mv * 200U) / 3300U);
    s_current_sensors.ambient_leak_ppm = (uint16_t)(((uint32_t)filtered_leak_mv * 50U) / 3300U);
    s_current_sensors.supply_voltage_mv = (uint16_t)((uint32_t)mv_supply * 10U);

    s_current_sensors.is_estop_normal = (hal_gpio_read(HAL_GPIO_IN_ESTOP_NC) == HAL_GPIO_LEVEL_HIGH);
    s_current_sensors.is_door_closed  = (hal_gpio_read(HAL_GPIO_IN_DOOR_INTERLOCK) == HAL_GPIO_LEVEL_HIGH);
    s_current_sensors.is_airflow_ok   = (hal_gpio_read(HAL_GPIO_IN_AIRFLOW_SW) == HAL_GPIO_LEVEL_HIGH);
    s_current_sensors.is_service_mode = (hal_gpio_read(HAL_GPIO_IN_SERVICE_KEY) == HAL_GPIO_LEVEL_HIGH);

    bool start_raw = (hal_gpio_read(HAL_GPIO_IN_START_BTN) == HAL_GPIO_LEVEL_LOW);
    if (start_raw) {
        s_start_btn_cnt += (uint16_t)elapsed_ms;
        if (s_start_btn_cnt >= DEBOUNCE_THRESHOLD_MS && !s_prev_start_raw) {
            s_current_sensors.is_start_btn_pressed = true;
            s_prev_start_raw = true;
        }
    } else {
        s_start_btn_cnt = 0;
        s_prev_start_raw = false;
    }

    bool stop_raw = (hal_gpio_read(HAL_GPIO_IN_STOP_BTN) == HAL_GPIO_LEVEL_LOW);
    if (stop_raw) {
        s_stop_btn_cnt += (uint16_t)elapsed_ms;
        if (s_stop_btn_cnt >= DEBOUNCE_THRESHOLD_MS && !s_prev_stop_raw) {
            s_current_sensors.is_stop_btn_pressed = true;
            s_prev_stop_raw = true;
        }
    } else {
        s_stop_btn_cnt = 0;
        s_prev_stop_raw = false;
    }

    const config_data_t *cfg = config_manager_get();
    if ((s_current_sensors.chamber_temp_c10 / 10) > (int16_t)cfg->max_temperature_c) {
        alarm_fault_set(FAULT_CODE_TEMP_OVER_LIMIT, FAULT_SEV_CRITICAL);
    }

    if (s_current_sensors.ambient_leak_ppm > cfg->ambient_leak_limit_ppm) {
        alarm_fault_set(FAULT_CODE_AMBIENT_GAS_LEAK, FAULT_SEV_CRITICAL);
    }
}

void sensor_driver_get_readings(sensor_data_t *out_data)
{
    if (out_data != NULL) {
        *out_data = s_current_sensors;
    }
}

void sensor_driver_clear_button_latches(void)
{
    s_current_sensors.is_start_btn_pressed = false;
    s_current_sensors.is_stop_btn_pressed  = false;
}
